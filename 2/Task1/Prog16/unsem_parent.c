#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/trace.h>
#include <sys/mman.h>

#define MYCODE 21

int main(int argc, char *argv[]) {
	pid_t 	childPID,
			PID = getpid();
	char str_message [45];
	sem_t * semap = mmap(NULL,sizeof(sem_t),
			PROT_READ|PROT_WRITE,
			MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// -----------------
	sprintf(str_message,
			"Program unsem_parent.c, father: PID %i",PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);
	// Creating semaphore
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
									"Father open SEMAPHORE");
	if(sem_init(semap, 1, 0) == -1){
		printf("sem_open: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	// Create child process
	childPID = fork();
	if(childPID == -1){
		printf("fork: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	if(childPID == 0){
		// Child code
		int i, temp = 0;
		printf("Program unsem_parent.c, son: PID %i\n",getpid());
		for(i=0;i<10000000;i++){
			// do something
			temp = temp + 1;
		}
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
								"Son post SEMAPHORE");
		sem_post(semap);
		return EXIT_SUCCESS;
	}else{
		// Father code
		printf("Waiting!\n");
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
							"Father wait SEMAPHORE");
		sem_wait(semap);
		wait(NULL);
		printf("Done!\n");
		return EXIT_SUCCESS;
	}
}
