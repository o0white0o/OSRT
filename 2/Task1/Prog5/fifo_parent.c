#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/trace.h>

#define MYCODE 21
#define NAMEFIFO "MyFIFO"

int main(int argc, char *argv[]) {
	pid_t 	childPID,
			PID = getpid();
	char str_message [45];
	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// -----------------
	sprintf(str_message,
			"Program fifo_parent.c, father: PID %i",PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);
	// Creating fifo
	system("ls -l");
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
							"Create FIFO");
	if(mkfifo(NAMEFIFO,S_IRWXU | S_IRWXG | S_IRWXO)==-1){
		printf("mkfifo: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	system("ls -l");
	// Open FIFO
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
								"Father open FIFO");
	int fileDesc = open(NAMEFIFO, O_RDWR);
	if(fileDesc == -1){
		printf("open fifo: %s\n", strerror(errno));
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
		char buffer[25];
		printf("Program fifo_parent.c, son: PID %i\n",getpid());
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
											"Son read FIFO");
		if(read(fileDesc,buffer,13)==-1){
			printf("thread open: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		printf("Message in son: %s\n", buffer);
		exit(EXIT_SUCCESS);
	}else{
		// Father code
		// Clearing str_message
		memset(str_message,0,sizeof(str_message));
		strcpy(str_message,"Test message");
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
							"Father write FIFO");
		if(write(fileDesc,str_message,sizeof(str_message))==-1){
			printf("write fifo: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		wait(NULL);
		if(unlink(NAMEFIFO)==-1){
			printf("unlink: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		printf("Done!\n");
		return EXIT_SUCCESS;
	}
}
