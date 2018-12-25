#include <inttypes.h>
#include <sys/neutrino.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <sys/trace.h>

#define MYCODE 21

void * threadFunction(void *arg);
int main (int argc, char *argv[]) {
	pthread_t threadID;
	uint64_t ntime = 2e9;

	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// --------------------------------

	// Create new thread
	printf("Starting second thread!\n");
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Creating new thread");
	if(pthread_create(&threadID, NULL, &threadFunction, NULL) == -1){
		printf("pthread_create: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	// Set timeout for pthread_join function
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Configure timeout for function pthread_join");
	if(TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_JOIN,
						NULL, &ntime, 0) == -1){
		printf("TimerTimeout: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Main thread join to second");
	if(pthread_join(threadID, NULL) == -1){
		printf("pthread_join: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Main thread unblock");
	printf("Main thread done!\n");
	return EXIT_SUCCESS;
}

void * threadFunction(void *arg) {
	sleep(6);
	printf("Second thread done!\n");
	pthread_exit(NULL);
}
