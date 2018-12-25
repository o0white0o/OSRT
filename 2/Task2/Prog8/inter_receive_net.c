#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/dispatch.h>
#include <fcntl.h>
#include <sys/trace.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/iofunc.h>

#define MYCODE 23
#define CHANNELNAME "ReceivePulse"

typedef struct{
	struct _pulse pulse_GNS;
	int data;
} msg_data_GNS;

int main (int argc, char * argv[]) {
	pid_t PID = getpid();
	char str_message [45];
	name_attach_t *puls_channel;
	int chid, countPulse, iter;
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

	// Connect to name channel
	puls_channel = name_attach(NULL, CHANNELNAME,
			NAME_FLAG_ATTACH_GLOBAL);
	if (puls_channel == NULL){
		printf("name_attach: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}else{
		printf("pulse_receive: Waiting message from GNS.\n");
		msg_data_GNS msgGNS;
		int receiveId = MsgReceive(puls_channel->chid, &msgGNS,
				sizeof(msgGNS), NULL);
		if(msgGNS.pulse_GNS.type == _IO_CONNECT){
			printf("pulse_receive: Received _IO_CONNECT"
					" from GNS.\n");
			MsgReply(receiveId, EOK, NULL,0);
		}else{
			printf("pulse_receive: Not received _IO_CONNECT from GNS.\n");
			return EXIT_FAILURE;
		}
	}

	sprintf(str_message, "Program inter_receive.c: PID %i chid %i\n",
			PID, puls_channel->chid);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);

	// Receiving 3 pulses
	for(iter=0; iter < countPulse; iter++) {
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Catch pulse from interrupt");
		MsgReceivePulse(puls_channel->chid, &pulseData, sizeof(pulseData), NULL);
		printf("inter_receive: Receive pulse, code %d, value %d\n",
							pulseData.code, pulseData.value.sival_int);
	}
	if(name_detach(puls_channel, NULL) == -1){
		printf("name_detach: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	printf("Finish receiving pulses!\n");
	return EXIT_SUCCESS;
}
