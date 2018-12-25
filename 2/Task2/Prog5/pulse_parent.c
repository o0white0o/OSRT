#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/trace.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>

#define MYCODE 21

int main(int argc, char *argv[]) {
	pid_t 	childPID,PID = getpid();
	char str_message [45];

	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);

	sprintf(str_message,
			"Program pulse_parent.c (parent): PID %i",PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);

	childPID = fork();
	if(childPID == -1){
		printf("fork: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	if(childPID == 0){
		// Child code
		// Clearing str_message
		memset(str_message,0,sizeof(str_message));
		childPID = getpid();
		sprintf(str_message,
					"Program msg_parent.c (son): PID %i",childPID);
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
		printf("%s\n",str_message);

		// Connect to channel
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Child connect to channel");
		int coid = ConnectAttach(ND_LOCAL_NODE, getppid(),1,_NTO_SIDE_CHANNEL,0);
		if(coid == -1){
			printf("ConnectAttach child: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}

		// Send pulse for message function
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Child send first pulse");
		if(MsgSendPulse(coid, -1,5,57) == -1){
			printf("MsgSendPulse 1: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		printf("Son not blocked!\n");

		// Send pulse for special function
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Child send second pulse");
		if(MsgSendPulse(coid, -1,21,89) == -1){
			printf("MsgSendPulse 2: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		ConnectDetach(coid);
		return EXIT_SUCCESS;
	}else{
		// Parent code
		int rcvId;
		// Create channel
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
							"Parent create channel");
		int chanlId = ChannelCreate(0);
		if(chanlId == -1){
			printf("ChannelCreate parent: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}

		// Receive pulse from child, message function
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
									"Parent receive pulse");
		struct _pulse pulseData;
		struct _msg_info msg_info;
		rcvId = MsgReceive(chanlId,(void *) &pulseData,sizeof(pulseData),&msg_info);
		if(rcvId == -1){
			printf("MsgReceive: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}else if(rcvId == 0){
			printf("Parent receive pulse (message function), code %d, value %d\n",
					pulseData.code, pulseData.value.sival_int);
		}else{
			printf("Pulse not received!\n");
		}

		// Receive pulse from child, special function
		if(MsgReceivePulse(chanlId,(void *) &pulseData, sizeof(pulseData), NULL) == -1){
			printf("MsgReceivePulse: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}else{
			printf("Parent receive pulse (special function), code %d, value %d\n",
					pulseData.code, pulseData.value.sival_int);
		}
		if(ChannelDestroy(chanlId) == -1){
			printf("ChannelDestroy: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		wait(NULL);
		printf("Done!\n");
		return EXIT_SUCCESS;
	}
}
