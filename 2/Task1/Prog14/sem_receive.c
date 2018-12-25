#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/trace.h>

#define MYCODE 21
#define NAMESEMAP "MySemaphore"

int main(int argc, char *argv[]) {
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
	// Creating SEMAPHORE
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
						"sem_receive.c open SEMAPHORE");
	sem_t *semap = sem_open(NAMESEMAP, O_CREAT,
			S_IRWXU|S_IRWXG|S_IRWXO, 0);
	if(semap == SEM_FAILED){
		printf("sem_open: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	// Starting second program
	system("./sem_send.run &");

	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
						"sem_receive wait SEMAPHORE");
	printf("Waiting!\n");
	sem_wait(semap);
	sem_close(semap);
	printf("Done!\n");
	return EXIT_SUCCESS;
}

