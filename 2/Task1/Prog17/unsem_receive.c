#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/trace.h>
#include <sys/mman.h>
#include <unistd.h>

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
			"Program unsem_receive.c: PID %i",PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);
	// Creating SEMAPHORE
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
						"unsem_receive.c open SEMAPHORE");
	int fd = shm_open(NAMESEMAP, O_RDWR|O_CREAT,
			S_IRWXU|S_IRWXG|S_IRWXO);
	if(fd == -1){
		printf("sem_open: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	// Setting length file with name -path or file descriptor
	//fd in length bytes.
	if(ftruncate(fd,sizeof(sem_t)) == -1){
		printf("ftruncate: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	sem_t *semap = mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_SHARED,
			fd, 0);
	if(sem_init(semap,1,0) == -1){
		printf("sem_init: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	// Starting second program
	system("./unsem_send.run &");

	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
						"sem_receive wait SEMAPHORE");
	printf("Waiting!\n");
	sem_wait(semap);
	sem_close(semap);
	shm_unlink(NAMESEMAP);
	printf("Done!\n");
	return EXIT_SUCCESS;
}

