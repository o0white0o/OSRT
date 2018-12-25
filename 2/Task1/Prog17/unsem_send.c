#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/trace.h>
#include <sys/mman.h>
#include <unistd.h>

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
			"Program unsem_receive.c: PID %i",PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);
	// Open Semaphore
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
								"unsem_receive open SEMAPHORE");
	int fd = shm_open(NAMESEMAP, O_RDWR, S_IRWXU|S_IRWXG|S_IRWXO);
	if(fd == -1){
		printf("sem_open: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	sem_t *semap = mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_SHARED,
			fd, 0);
	for(i=0;i<1000000;i++){
		// do something
		temp = temp + 1;
	}
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
						"queue_send write QUEUE");
	sem_post(semap);
	return EXIT_SUCCESS;
}
