#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/netmgr.h>
#include <unistd.h>
#include <string.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/trace.h>
#include <pthread.h>
#include <errno.h>

#define MYCODE 21
#define NAME_CHANNEL "ServerTime"
#define SEND_DATA_TO_SERVER 1
#define END_WORK 2
#define COUNT_CLIENTS 6
#define CONNECT_SERVER 3

// Structure for send message
typedef struct{
	struct _pulse pulseData;
	float data;
} message_t;

// Clients are realized as functions
void *clientFunction(void *arg);

int main(int argc, char *argv[]) {
	pthread_t clientsId[COUNT_CLIENTS];
	int iter;
	// Set name for main thread
	pthread_setname_np(pthread_self(),"Main client");

	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// --------------------------------

	for(iter = 1; iter<=COUNT_CLIENTS; iter++){
		// Creating new client
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Starting another client");
		pthread_create(&clientsId[iter-1],NULL, &clientFunction,(void*)iter);
	}
	// Wait until clients finish work
	for(iter = 0; iter<COUNT_CLIENTS; iter++){
		pthread_join(clientsId[iter],0);
	}
	printf("Done!\n");
	return EXIT_SUCCESS;
}

void *clientFunction(void *arg){
	int idClient = (int)arg;
	int coid, iter, resSend,
	   	   failSend = 0,
	   	   failCalc = 0;
	struct timespec timeStart, timeEnd;
	char name[32];
	double timeWork;
	message_t msgSend, msgRec;
	// Max wait time for client - 4 ms
	uint64_t waitTime = 4e6*idClient;
	// Set name for each client thread
	sprintf(name,"client %i",idClient);
	pthread_setname_np(pthread_self(),name);
	printf("Starting %s.\n",name);
	// Connect to server channel
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Client open channel to server");
	coid = name_open(NAME_CHANNEL, NAME_FLAG_ATTACH_GLOBAL);
	if(coid == -1){
		printf("name_attach: %s\n", strerror(errno));
		pthread_exit((void*)1);
	}
	msgSend.pulseData.code = SEND_DATA_TO_SERVER;
	// Get start time for calculate work time
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Get start time");
	clock_gettime(CLOCK_REALTIME,&timeStart);
	// Send points in range [-50:50] with step 1
	for(iter = -50; iter<50; iter++){
		// Configure timeout on SEND and REPLY block
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Configure timeout for SEND and REPLY");
		TimerTimeout(CLOCK_REALTIME,_NTO_TIMEOUT_SEND |
				_NTO_TIMEOUT_REPLY, NULL, &waitTime, 0);
		// Send message
		msgSend.data = (float)iter;
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Client send data");
		resSend = MsgSend(coid,&msgSend, sizeof(msgSend),
						&msgRec, sizeof(msgRec));
		// Time is out?
		if(resSend == -1 && errno == EINTR){
			TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Send failed");
			failSend++;
			iter--;
		}else{
			// Verify calculation
			float checkCalc = (float)iter*2+12;
			if(msgRec.data != checkCalc)
				failCalc++;
		}
		// Connect to server is good
		if(resSend == CONNECT_SERVER){
			iter--;
		}
		// Request period is idClient * 1 ms
		usleep(1000*idClient);
	}
	// Get finish time for calculate work time
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Get end time");
	clock_gettime(CLOCK_REALTIME, &timeEnd);
	// Calculate work time
	timeWork = (double)(timeEnd.tv_sec - timeStart.tv_sec) +
			(double)(timeEnd.tv_nsec - timeStart.tv_nsec)/1e9;
	// Display statistic information
	printf("Client %i: end work! Fail send %i, Fail calculate %i, Work time %.6f sec\n",
			idClient, failSend, failCalc, timeWork);
	// Say to server that work is finish
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Send pulse");
	MsgSendPulse(coid,-1,END_WORK,0);
	name_close(coid);
	pthread_exit(0);
}
