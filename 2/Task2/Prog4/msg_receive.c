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
#define CHANNELNAME "ReceiveMsg"

typedef struct{
	struct _pulse pulse_GNS;
	int data;
} msg_data_GNS;

typedef union {
char msg_buff[256];
} Msg_T;


int main( int argc, char *argv[] ){
	pid_t PID = getpid();
	char str_message [45];
	int rcv_id;
	Msg_T msg;
	name_attach_t* msg_chanel;
	struct _msg_info msg_info;

	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// --------------------------------
	sprintf(str_message, "Program msg_receive.c: PID %i",PID);
	printf("%s\n",str_message);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);

	//system("on -f /net/QNX2 ./msg_send.run -g &");

	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "msg_receive attach name to channel");
	// Creating name channel
	msg_chanel = name_attach(NULL,CHANNELNAME, NAME_FLAG_ATTACH_GLOBAL);
	if (msg_chanel == NULL){
		printf("name_attach: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}else{
		printf("msg_receive: Waiting message from GNS.\n");
		msg_data_GNS msgGNS;
		int receiveId = MsgReceive(msg_chanel->chid, &msgGNS, sizeof(msgGNS), NULL);
		if(msgGNS.pulse_GNS.type == _IO_CONNECT){
			printf("msg_receive: Received _IO_CONNECT from GNS.\n");
			MsgReply(receiveId, EOK, NULL,0);
		}else{
			printf("msg_receive: Not received _IO_CONNECT from GNS.\n");
			return EXIT_FAILURE;
		}
	}
	// Receive message from another node
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "msg_receive receive message");
	rcv_id = MsgReceive( msg_chanel->chid, &msg, sizeof(msg), &msg_info );
	if(rcv_id == -1){
		printf("MsgReceive: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	printf("msg_receive: Received message: %s\n", msg.msg_buff);
	strcpy(msg.msg_buff, "Message received!");
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "msg_receive send reply");
	MsgReply(rcv_id, 0, &msg, sizeof(msg));
	name_detach(msg_chanel, 0);
	printf("Done!\n");
	return EXIT_SUCCESS;
}
