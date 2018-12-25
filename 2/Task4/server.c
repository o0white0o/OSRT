#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/dispatch.h>
#include <sys/neutrino.h>
#include <sys/trace.h>
#include <errno.h>
#include <sys/netmgr.h>

#define MYCODE 21
#define CHANNELNAME "agentSystem"
#define COUNTAGENT 5

// Structure sending message
typedef struct{
	struct _pulse pulseData;
	int idSender;
	char data[128];
	int isAgent;
} message;

// Agent function
void *agentFun(void *arg);

int main (int argc, char * argv[]) {
	pid_t PID = getpid();
	char str_message [45];
	int receiveId = 0,
		connetAgentId[COUNTAGENT],
		numAgent = -1,
		iter = 0;
	pthread_t agentThreads[COUNTAGENT];
	name_attach_t *chanServer;
	message incomMsg;

	// Creating to name channel
	chanServer = name_attach(NULL, CHANNELNAME, 0);
	if(chanServer == NULL){
		printf("Server: name_attach: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// --------------------------------

	sprintf(str_message, "Program server.c: PID %i\n", PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);

	// Starting agents
	for(iter=0; iter<COUNTAGENT; iter++){
		if(pthread_create(&agentThreads[iter], NULL,
				&agentFun, (void *)iter) == -1){
			printf("Server: pthread_create: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
	}
	// Main server work
	while(1){
		// Receive all messages and pulses
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Server receive something");
		receiveId = MsgReceive(chanServer->chid, &incomMsg,
				sizeof(incomMsg), NULL);
		if(receiveId > 0){
			// Incoming message
			if(incomMsg.isAgent == 1){
				// From agent
				numAgent++;
				connetAgentId[numAgent] = receiveId;
				printf("Connect agent %i\n",numAgent+1);
			}else if(incomMsg.isAgent == 2){
				// From client
				if(numAgent < 0){
					// Reply client, because no free agents
					printf("No free agents!\n");
					sprintf(incomMsg.data,"No free agents!");
					TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Server send to client: no free agents");
					if(MsgReply(receiveId,0,
							&incomMsg,sizeof(incomMsg)) == -1){
						printf("Server: MsgReply: %s\n", strerror(errno));
						return EXIT_FAILURE;
					}
				}else{
					// Reply agent
					incomMsg.idSender = receiveId;
					TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Server reply to agent");
					MsgReply(connetAgentId[numAgent],0,
							&incomMsg,sizeof(incomMsg));
					numAgent--;
				}
			}
		}
		// Disconnect client
		if(receiveId == 0 && incomMsg.pulseData.code == 21){
			// Incoming pulse
			for(iter=0; iter<COUNTAGENT; iter++){
				incomMsg.isAgent = -1;
				MsgReply(connetAgentId[iter],0,
						&incomMsg,sizeof(incomMsg));
			}
			break;
		}
	}
	// Wait for stop all agents
	for(iter=0; iter<COUNTAGENT; iter++){
		pthread_join(agentThreads[iter],0);
	}
	// Close name channel
	if(name_detach(chanServer,0) == -1){
		printf("Server: name_detach: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	printf("Server: Done!\n");
	return EXIT_SUCCESS;
}

void *agentFun(void *arg){
	int numAgent = (int)arg + 1,
			iter;
	char name[32];
	char str[32];
	message msg, msg_reply;
	sprintf(name,"Agent %i",numAgent);
	pthread_setname_np(pthread_self(),name);
	// Connect to name channel
	int chanID = name_open(CHANNELNAME,0);
	if(chanID <0){
		printf("Agent: name_open: %s\n", strerror(errno));
		pthread_exit(0);
	}
	msg.idSender = numAgent;
	msg.isAgent = 1;
	sprintf(msg.data,"Ready");
	while(1){
		// Send message to server (Ready to work)
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Agent send ready message to server");
		MsgSend(chanID,(void*)&msg,sizeof(msg),&msg_reply,sizeof(msg_reply));
		if(msg_reply.isAgent == -1)
			// All clients are disconnect
			break;
		printf("%s: Reply message: %s\n",name,msg_reply.data);
		for(iter=0; iter<20000000; iter++){
			// Calculating something
		}
		// Send reply message to client
		sprintf(msg_reply.data,"Hello client! I'am agent %i",numAgent);
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Agent send reply message to client");
		MsgReply(msg_reply.idSender,0,&msg_reply,sizeof(msg_reply));
	}
	// Disconnect name channel
	if(name_close(chanID) == -1){
		printf("Agent: name_close: %s\n", strerror(errno));
		pthread_exit(0);
	}
	pthread_exit(0);
}
