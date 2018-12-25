#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/trace.h>

#define MYCODE 21
#define NAMESEMAP "MySemaphore"

pthread_t threadID;

void * secondThread(void * arg);

int main(int argc, char *argv[]) {
	pid_t 	PID = getpid();
	char str_message [45];
	sem_t * semap;

	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// -----------------
	sprintf(str_message,
			"Program sem_thread.c: PID %i",PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);

	// Creating semaphore
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
							"First thread open semaphore");
	semap = sem_open(NAMESEMAP, O_CREAT,
			S_IRWXU | S_IRWXG | S_IRWXO, 0);
	if(semap==SEM_FAILED){
		printf("sem_open: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	// Starting new thread
	pthread_create(&threadID,NULL,secondThread,NULL);
	//Waiting thread
	printf("Waiting!\n");
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
								"First thread wait semaphore");
	sem_wait(semap);
	sem_close(semap);

	pthread_join(threadID,NULL);
	printf("Done!\n");
	return EXIT_SUCCESS;
}

void * secondThread(void * arg){
	int i, temp = 0;
	sem_t * semap;
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE + 2,
							"Second thread open semaphore");
	semap = sem_open(NAMESEMAP, 0, S_IRWXU|S_IRWXG|S_IRWXO, 0);
	if(semap==SEM_FAILED){
		printf("sem_open: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	for(i=0;i<10000000;i++){
		// do something
		temp = temp + 1;
	}
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE + 2,
								"Second thread post semaphore");
	sem_post(semap);
	pthread_exit(NULL);
}
