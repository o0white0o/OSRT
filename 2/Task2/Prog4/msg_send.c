#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/dispatch.h>
#include <sys/trace.h>

#define MYCODE 23
#define CHANNELNAME "ReceiveMsg"

int main( int argc, char *argv[] ){
	pid_t 	childPID,PID = getpid();
	char str_message [45];
	int chanId = -1;
	char buffer[80];

	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// --------------------------------

	sprintf(str_message, "Program msg_send.c: PID %i",PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);

	memset(str_message,0,sizeof(str_message));
	strcpy(str_message,"Test message!");
	while(chanId < 0){
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "msg_send try open channel");
		chanId = name_open(CHANNELNAME, NAME_FLAG_ATTACH_GLOBAL);
		if(chanId == -1){
			printf("name_open: %s\n", strerror(errno));
			sleep(1);
		}
	}

	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "msg_send send message");
	if(MsgSend(chanId,str_message,sizeof(str_message),
				buffer, sizeof(buffer)) == -1){
		printf("MsgSend: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	printf("Reply message: %s \n", buffer);
	name_close(chanId);
	return EXIT_SUCCESS;
}
