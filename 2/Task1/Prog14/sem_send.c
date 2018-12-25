#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/trace.h>

#define MYCODE 23
#define NAMESEMAP "MySemaphore"

int main(int argc, char *argv[]) {
	int i, temp = 0;
	pid_t 	PID = getpid();
	char str_message [45];
	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// -----------------
	sprintf(str_message,
			"Program sem_receive.c: PID %i",PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);
	// Open FIFO
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
								"sem_receive open SEMAPHORE");
	sem_t *semap = sem_open(NAMESEMAP, 0, S_IRWXU|S_IRWXG|S_IRWXO, 0);
	if(semap == SEM_FAILED){
		printf("sem_open: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	for(i=0;i<10000000;i++){
		// do something
		temp = temp + 1;
	}
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
						"queue_send write QUEUE");
	sem_post(semap);
	return EXIT_SUCCESS;
}
