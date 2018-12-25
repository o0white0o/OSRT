#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/dispatch.h>
#include <sys/trace.h>

#define MYCODE 21
#define CHANNELNAME "ReceiveMsg"

typedef union {
char msg_buff[256];
} Msg_T;


int main( int argc, char *argv[] ){
	pid_t PID = getpid();
	char str_message [45];
	char comForRun [128];
	int rcv_id;
	Msg_T msg;
	name_attach_t* msg_chanel;
	struct _msg_info msg_info;
	if(argc < 2) {
		printf("Missing argument (type work).\n");
		return EXIT_FAILURE;
	}

	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// --------------------------------
	sprintf(str_message, "Program msg_receive.c: PID %i",PID);
	printf("%s\n",str_message);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	unsigned int flag;
	if(strcmp(argv[1],"-g" ) == 0){
		flag = NAME_FLAG_ATTACH_GLOBAL;
		sprintf(comForRun,"on -f /net/QNX2 ./msg_send.run -g &");
	}else{
		flag = 0;
		sprintf(comForRun,"./msg_send.run -l &");
	}

	system(comForRun);

	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "msg_receive attach name to channel");
	msg_chanel = name_attach(NULL,CHANNELNAME, flag);
	if (msg_chanel == NULL){
		printf("name_attach: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "msg_receive receive message");
	rcv_id = MsgReceive( msg_chanel->chid, &msg, sizeof(msg), &msg_info );
	if(rcv_id == -1){
		printf("MsgReceive: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	printf("Received message: %s\n", msg.msg_buff);
	strcpy(msg.msg_buff, "Message received!");
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "msg_receive send reply");
	MsgReply(rcv_id, 0, &msg, sizeof(msg));
	name_detach(msg_chanel, 0);
	return EXIT_SUCCESS;
}
