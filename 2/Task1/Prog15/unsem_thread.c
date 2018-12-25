#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/trace.h>

#define MYCODE 21

pthread_t threadID;
sem_t semap;

void * secondThread(void * arg);

int main(int argc, char *argv[]) {
	pid_t 	PID = getpid();
	char str_message [45];

	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// -----------------
	sprintf(str_message,
			"Program unsem_thread.c: PID %i",PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);

	// Creating semaphore
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
							"First thread open semaphore");
	if(sem_init(&semap,0,0)==-1){
		printf("sem_open: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	// Starting new thread
	pthread_create(&threadID,NULL,secondThread,NULL);
	//Waiting thread
	printf("Waiting!\n");
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
								"First thread wait semaphore");
	sem_wait(&semap);

	pthread_join(threadID,NULL);
	printf("Done!\n");
	return EXIT_SUCCESS;
}

void * secondThread(void * arg){
	int i, temp = 0;
	for(i=0;i<10000000;i++){
		// do something
		temp = temp + 1;
	}
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE + 2,
								"Second thread post semaphore");
	sem_post(&semap);
	pthread_exit(NULL);
}
