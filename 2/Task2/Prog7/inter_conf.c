#include <stdio.h>
#include <stdlib.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/trace.h>

#define MYCODE 21

const struct sigevent* progTimer (void *aread, int id);
struct sigevent event;
volatile int tic=0;
volatile int counter=0;

int main (int argc, char * argv[]) {
	pid_t 	PID = getpid();
	char str_message [45];
	int chid, rec_nd, coid, ifid, iter;
	pid_t target;
	struct _pulse pulseData;
	char buffer[80];
	time_t ct;
	if(argc < 4 ) {
		printf("Missing arguments! (countPulse, PID, chid)\n");
		return EXIT_SUCCESS;
	}
	counter = atoi(argv[1]);

	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// --------------------------------

	sprintf(str_message, "Program inter_conf.c: PID %i",PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);

	target = atoi(argv[2]);
	chid = atoi(argv[3]);
	printf("Sending %i pulses to pid %i.\n", counter, target);
	coid = ConnectAttach( ND_LOCAL_NODE, target, chid, _NTO_SIDE_CHANNEL, 0);
	if(coid == -1){
		printf("ConnectAttach: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	if(ThreadCtl(_NTO_TCTL_IO, 0) == -1){
		printf("ThreadCtl: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	SIGEV_PULSE_INIT(&event, coid, SIGEV_PULSE_PRIO_INHERIT, 23, 21);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Attach interrupt");
	ifid = InterruptAttach(0, progTimer, (void*)(&counter),
			sizeof(counter), 0 | _NTO_INTR_FLAGS_TRK_MSK);
	if(ifid == -1){
		printf("InterruptAttach: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	for(iter=0; iter < counter + 2; iter++) {
		sleep(1);
	}
	if(InterruptDetach(ifid) == -1){
		printf("InterruptDetach: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

const struct sigevent *progTimer(void *counter, int id) {
	++tic;
	if (tic == 1000) {
		--(*(int*)counter);
		tic=0;
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Send sigevent");
		return &event;
	}
	else return NULL;
}
