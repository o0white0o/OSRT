#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/dispatch.h>
#include <sys/trace.h>

#define MYCODE 23
#define CHANNELNAME "ReceivePulse"

int main( int argc, char *argv[] ){
	pid_t 	PID = getpid();
	char str_message [45];
	int chanId = -1;
	char buffer[80];

	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// --------------------------------

	sprintf(str_message, "Program pulse_send.c: PID %i",PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);

	while(chanId < 0){
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "pulse_send try open channel");
		chanId = name_open(CHANNELNAME, NAME_FLAG_ATTACH_GLOBAL);
		if(chanId == -1){
			printf("name_open: %s\n", strerror(errno));
			sleep(1);
		}
	}

	// Send pulse for message function
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Child send first pulse");
	if(MsgSendPulse(chanId, -1,5,57) == -1){
		printf("MsgSendPulse 1: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	printf("Son not blocked!\n");

	// Send pulse for special function
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Child send second pulse");
	if(MsgSendPulse(chanId, -1,21,89) == -1){
		printf("MsgSendPulse 2: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	name_close(chanId);
	return EXIT_SUCCESS;
}
