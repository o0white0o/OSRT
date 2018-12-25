#include <stdio.h>
#include <stdlib.h>
#include <sys/dispatch.h>
#include <sys/netmgr.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/iofunc.h>
#include <unistd.h>
#include <sys/trace.h>

#define MYCODE 21
#define CHANNELNAME "ReceivePulse"

const struct sigevent* progTimer (void *aread, int id);
struct sigevent event;
volatile int tic=0;
volatile int counter=0;

int main (int argc, char * argv[]) {
	pid_t 	PID = getpid();
	char str_message [45];
	int chid = -1,
		rec_nd, coid, ifid, iter;
	struct _pulse pulseData;
	char buffer[80];
	time_t ct;
	if(argc < 2 ) {
		printf("Missing arguments! (countPulse)\n");
		return EXIT_SUCCESS;
	}
	counter = atoi(argv[1]);

	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// --------------------------------

	sprintf(str_message, "Program inter_conf_net.c: PID %i",PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);
	if(ThreadCtl(_NTO_TCTL_IO, 0) == -1){
		printf("ThreadCtl: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	// Try connect to channel ReceivePulse
	while(chid <= 0){
		chid = name_open(CHANNELNAME, NAME_FLAG_ATTACH_GLOBAL);
		sleep(1);
	}
	if(chid == -1){
		printf("ConnectAttach: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	printf("Sending %i pulses to channel id %i.\n", counter, chid);
	// Configurate event to send pulse
	SIGEV_PULSE_INIT(&event, chid, SIGEV_PULSE, 23, 21);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Attach interrupt");
	// Attach progTimer to timer interrupt
	ifid = InterruptAttach(0, progTimer, (void*)(&counter),
			sizeof(counter), 0 | _NTO_INTR_FLAGS_TRK_MSK);
	if(ifid == -1){
		printf("InterruptAttach: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	// Wait 1 second to interrupt send pulse, then send pulse out of program
	while(counter != 0){
		printf("Interrupt is working, but don't send pulse. Counter now %i\n",counter);
		sleep(1);
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Process "
				"inter_conf_net send pulse");
		MsgSendPulse(chid,-1,1,1);
	}
	if(InterruptDetach(ifid) == -1){
		printf("InterruptDetach: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	name_close(chid);
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
