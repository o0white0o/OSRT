#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/trace.h>

#define MYCODE 21
#define NAMEQUEUE "MyQueue"

int main(int argc, char *argv[]) {
	pid_t 	childPID,
			PID = getpid();
	mqd_t mes;
	char str_message [45];
	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// -----------------
	sprintf(str_message,
			"Program queue_parent.c, father: PID %i",PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);
	// Creating queue
	system("ls -l");
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
									"Father open QUEUE");
	mes = mq_open(NAMEQUEUE, O_RDWR | O_CREAT,
			S_IRWXU | S_IRWXG | S_IRWXO, NULL);
	if(mes == -1){
		printf("mq_open: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	system("ls -l");

	// Create child process
	childPID = fork();
	if(childPID == -1){
		printf("fork: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	if(childPID == 0){
		// Child code
		char buffer[15];
		printf("Program queue_parent.c, son: PID %i\n",getpid());
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
											"Son read QUEUE");
		if(mq_receive(mes, buffer, 4096, NULL)==-1){
			printf("mq_receive: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		printf("Son incoming message: %s\n", buffer);
		return EXIT_SUCCESS;
	}else{
		// Father code
		// Clearing str_message
		memset(str_message,0,sizeof(str_message));
		strcpy(str_message,"Test message");
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
							"Father write QUEUE");
		if(mq_send(mes, str_message, sizeof(str_message), 0)==-1){
			printf("write QUEUE: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		wait(NULL);
		if(mq_unlink(NAMEQUEUE)==-1){
			printf("mq_unlink: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		printf("Done!\n");
		return EXIT_SUCCESS;
	}
}
