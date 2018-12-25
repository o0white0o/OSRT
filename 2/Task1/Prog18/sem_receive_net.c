#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/trace.h>

#define MYCODE 21
#define NAMESEMAP "/net/QNX/home/MySemaphore"

int main(int argc, char *argv[]) {
	pid_t 	PID = getpid();
	char str_message [45];
	char comForRun [128];
	if(argc < 2){
		printf("Missing argument (Node).\n");
		return EXIT_FAILURE;
	}
	sprintf(comForRun,"on -f /net/%s ./sem_send_net.run &",argv[1]);
	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// -----------------
	sprintf(str_message,
			"Program sem_receive_net.c: PID %i",PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);
	// Creating SEMAPHORE
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
						"sem_receive_net.c open SEMAPHORE");
	sem_t *semap = sem_open(NAMESEMAP, O_CREAT|O_EXCL,
			S_IRWXU|S_IRWXG|S_IRWXO, 0);
	if(semap == SEM_FAILED){
		printf("sem_open: %s\n", strerror(errno));
		sem_unlink(NAMESEMAP);
		return EXIT_FAILURE;
	}

	// Starting second program at another QNX machine
	system(comForRun);

	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
						"sem_receive wait SEMAPHORE");
	printf("Waiting!\n");
	sem_wait(semap);
	sem_unlink(NAMESEMAP);
	printf("Done!\n");
	return EXIT_SUCCESS;
}

