#include <stdio.h>
#include <stdlib.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <fcntl.h>
#include <sys/trace.h>
#include <errno.h>

#define MYCODE 23

int main (int argc, char * argv[]) {
	pid_t PID = getpid();
	char str_message [45];
	int chid, coid, countPulse, iter;
	struct _pulse pulseData;
	if(argc < 2 ) {
		printf("Missing arguments! (countPulse)\n");
		return EXIT_SUCCESS;
	}
	countPulse = atoi(argv[1]);
	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// --------------------------------

	chid = ChannelCreate(0);
	if(chid == -1){
		printf("ChannelCreate: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	sprintf(str_message, "Program inter_receive.c: PID %i chid %i",PID, chid);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);

	for(iter=0; iter < countPulse; iter++) {
		MsgReceivePulse(chid, &pulseData, sizeof(pulseData), NULL);
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Catch pulse from interrupt");
		printf("inter_receive: Receive pulse, code %d, value %d\n",
							pulseData.code, pulseData.value.sival_int);
	}
	return EXIT_SUCCESS;
}
