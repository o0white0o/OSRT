#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <process.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <unistd.h>
#include <string.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/trace.h>
#include <errno.h>

#define MYCODE 23
#define NAME_CHANNEL "ServerTime"
#define SEND_DATA_TO_SERVER 1
#define END_WORK 2
#define COUNT_CLIENTS 5
#define CONNECT_SERVER 3

// Structure for send message
typedef struct{
	struct _pulse pulseData;
	float data;
} message_t;

int main(int argc, char *argv[]) {
	int countClients = 0,
	    receiveId;
	name_attach_t *serverChannel;
	message_t msgReceive, msgReply;

	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// --------------------------------

	printf("Server started, PID %i\n",(int)getpid());
	// Create name channel
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Create name channel");
	serverChannel = name_attach(0,NAME_CHANNEL,NAME_FLAG_ATTACH_GLOBAL);
	if (serverChannel == NULL){
		printf("name_attach: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	for(;;){
		// Receive message from client
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Receive message from client");
		receiveId = MsgReceive(serverChannel->chid,
				&msgReceive,sizeof(msgReceive),0);
		// New client connected ?
		if(msgReceive.pulseData.type == _IO_CONNECT){
			printf("Connect new client\n");
			countClients++;
			MsgReply(receiveId,CONNECT_SERVER,0,0);
			//continue;
		}
		if(receiveId > 0){
			// Incoming message is data which use in calculations?
			if(msgReceive.pulseData.code == SEND_DATA_TO_SERVER){
				msgReply.data = msgReceive.data * 2 + 12;
				// Simulating large computing
				usleep(12000);
				// Reply answer
				MsgReply(receiveId,0,&msgReply, sizeof(msgReply));
			}
		}
		// Incoming message is pulse with code that client disconnect
		if(receiveId == 0 && msgReceive.pulseData.code == END_WORK){
			countClients--;
			printf("One client finish and disconnect.\n");
			if(countClients == 0)
				break;
		}
	}
	printf("Server done\n");
	name_detach(serverChannel,0);
	return 0;
}
