#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/neutrino.h>
#include <sys/trace.h>
#include <errno.h>
#include <sys/netmgr.h>

#define MYCODE 23
#define CHANNELNAME "agentSystem"
#define COUNTCLIENTS 7

// Structure sending message
typedef struct{
	struct _pulse pulseData;
	int idSender;
	char data[128];
	int isAgent;
} message;

// Client function
void *clientFun(void* arg);

int main (int argc, char * argv[]) {
	pid_t PID = getpid();
	char str_message [45];
	pthread_t clientThreads[COUNTCLIENTS];

	int iter = 0;

	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// --------------------------------

	// Connect to name channel
	int chanId = name_open(CHANNELNAME, 0);
	if(chanId <= 0){
		printf("Main client: name_open: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	sprintf(str_message, "Program client.c: PID %i",PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);
	// Starting client threads
	for(iter=0; iter<COUNTCLIENTS; iter++){
		pthread_create(&clientThreads[iter], NULL,
				&clientFun, (void *)iter);
	}
	// Waiting clients
	for(iter=0; iter<COUNTCLIENTS; iter++){
		pthread_join(clientThreads[iter],NULL);
	}
	// Stop server
	MsgSendPulse(chanId,-1,21,1);
	// Disconnect from name channel
	if(name_close(chanId) == -1){
		printf("Main client: name_close: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	printf("Client: Done!\n");
	return EXIT_SUCCESS;
}

void *clientFun(void* arg){
	int numClient = (int)arg + 1;
	char name[32];
	message msg, msg_reply;
	msg.idSender = numClient;
	msg.isAgent = 2;
	int chanId = name_open(CHANNELNAME, 0);
	if(chanId <= 0){
		printf("Main client: name_open: %s\n", strerror(errno));
		pthread_exit(0);
	}
	sprintf(msg.data,"Hello agent! I'am client %i",numClient);
	sprintf(name,"Client %i",numClient);
	pthread_setname_np(pthread_self(),name);
	// Send message to server
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Client send message to server");
	MsgSend(chanId,&msg,sizeof(msg),&msg_reply,sizeof(msg_reply));
	printf("%s: Reply message: %s\n",name,msg_reply.data);
	if(name_close(chanId) == -1){
		printf("Main client: name_close: %s\n", strerror(errno));
		pthread_exit(0);
	}
	pthread_exit(0);
}
