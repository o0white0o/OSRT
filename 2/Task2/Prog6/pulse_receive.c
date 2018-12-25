#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/dispatch.h>
#include <sys/trace.h>
#include <sys/types.h>
#include <sys/iofunc.h>

#define MYCODE 21
#define CHANNELNAME "ReceivePulse"

typedef struct{
	struct _pulse pulse_GNS;
	int data;
} msg_data_GNS;


int main( int argc, char *argv[] ){
	pid_t PID = getpid();
	char str_message [45];
	name_attach_t* msg_chanel;
	struct _msg_info msg_info;
	struct _pulse pulseData;

	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// --------------------------------
	sprintf(str_message, "Program pulse_receive.c: PID %i",PID);
	printf("%s\n",str_message);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);

	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "pulse_receive attach name to channel");
	// Creating name channel
	msg_chanel = name_attach(NULL,CHANNELNAME, NAME_FLAG_ATTACH_GLOBAL);
	if (msg_chanel == NULL){
		printf("name_attach: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}else{
		printf("pulse_receive: Waiting message from GNS.\n");
		msg_data_GNS msgGNS;
		int receiveId = MsgReceive(msg_chanel->chid, &msgGNS, sizeof(msgGNS), NULL);
		if(msgGNS.pulse_GNS.type == _IO_CONNECT){
			printf("pulse_receive: Received _IO_CONNECT from GNS.\n");
			MsgReply(receiveId, EOK, NULL,0);
		}else{
			printf("pulse_receive: Not received _IO_CONNECT from GNS.\n");
			return EXIT_FAILURE;
		}
	}

	// Receive message from another node (message function)
	int rcvId = MsgReceive(msg_chanel->chid,(void *) &pulseData,sizeof(pulseData),&msg_info);
	if(rcvId == -1){
		printf("MsgReceive: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}else if(rcvId == 0){
		printf("Parent receive pulse (message function), code %d, value %d\n",
				pulseData.code, pulseData.value.sival_int);
	}else{
		printf("Pulse not received!\n");
	}

	// Receive message from another node (special function)
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "pulse_receive receive message");
	if(MsgReceivePulse(msg_chanel->chid, (void *) &pulseData,
			sizeof(pulseData), &msg_info ) == -1){
		printf("MsgReceivePulse: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	printf("Parent receive pulse (special function), code %d, value %d\n",
				pulseData.code, pulseData.value.sival_int);

	name_detach(msg_chanel, 0);
	printf("Done!\n");
	return EXIT_SUCCESS;
}
