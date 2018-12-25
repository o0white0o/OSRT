#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/trace.h>
#include <signal.h>

#define MYCODE 13

pthread_t 	threadID_1,
			threadID_2;
int 		stop = 0;

void signalHandler(int arg);

void * secondThread(void * arg);

void * thirdThread(void * arg);

int main(int argc, char *argv[]) {
	int pid = getpid();
	char str_message [45];
	sprintf(str_message,
			"Prog. signal.c: PID %i",pid);
	printf("%s\n",str_message);
	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// -----------------
	pthread_create(&threadID_1, NULL, secondThread, NULL);
	pthread_create(&threadID_2, NULL, thirdThread, NULL);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	pthread_join(threadID_1, NULL);
	printf("Done!\n");
	return EXIT_SUCCESS;
}

void signalHandler(int arg){
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Signal ctached!");
	printf("Signal catched!\n");
	stop = 1;
}

void * secondThread(void * arg){
	signal(SIGUSR1, signalHandler);
	for(;;){
		if(stop != 0)
			break;
	}
	pthread_exit(EXIT_SUCCESS);
}

void * thirdThread(void * arg){
	pthread_kill(threadID_1, SIGUSR1);
	pthread_exit(EXIT_SUCCESS);
}
