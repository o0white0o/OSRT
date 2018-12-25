#include <stdlib.h>
#include <stdio.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <sys/dispatch.h>
#include <sys/trace.h>
#include <errno.h>
#include <signal.h>

#define MYCODE 23
#define NAME_CHANNEL "Watcher"
#define SEND_DATA_TO_SERVER 1
#define END_WORK 2
#define CONNECT_SERVER 3
#define SERVER_AVAILABLE 4

// Structure for send message
typedef struct{
	struct _pulse pulseData;
	float data;
} message_t;

int main(int argc, char *argv[]) {
	int receiveId;
	name_attach_t *serverChannel;
	message_t msgReceive, msgReply;

	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// --------------------------------

	printf("Server for watcher started, PID %i\n",(int)getpid());
	// Create name channel
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Create name channel");
	serverChannel = name_attach(0,NAME_CHANNEL,NAME_FLAG_ATTACH_GLOBAL);
	if (serverChannel == NULL){
		printf("name_attach: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	for(;;){
		// Receive message from watcher
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Receive message from watcher");
		receiveId = MsgReceive(serverChannel->chid,
				&msgReceive,sizeof(msgReceive),0);
		// Watcher connected ?
		if(msgReceive.pulseData.type == _IO_CONNECT){
			printf("Connect watcher!\n");
			MsgReply(receiveId,CONNECT_SERVER,0,0);
			//continue;
		}
		if(receiveId > 0){
			// Incoming message is command to monitor node?
			if(msgReceive.pulseData.code == SEND_DATA_TO_SERVER){
				MsgReply(receiveId,SERVER_AVAILABLE, 0, 0);
			}
		}
		// Incoming message is pulse with code that watcher disconnect
		if(receiveId == 0 && msgReceive.pulseData.code == END_WORK){
			printf("Watcher disconnect, server turn-off.\n");
			break;
		}
	}
	printf("Server done\n");
	name_detach(serverChannel,0);
	return 0;
}
