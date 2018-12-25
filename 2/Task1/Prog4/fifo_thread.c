#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/trace.h>

#define MYCODE 21
#define NAMEFIFO "MyFIFO"

void * secondThread(void * arg);

int main(int argc, char *argv[]) {
	pid_t PID = getpid();
	pthread_t threadID;
	char str_message [45];
	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// -----------------
	sprintf(str_message,
			"Program fifo_thread.c: PID %i",PID);
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
	// Creating thread
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
						"Create second thread");
	pthread_create(&threadID,NULL,secondThread,NULL);
	// Open FIFO
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
								"Main open FIFO");
	int fileDesc = open(NAMEFIFO, O_WRONLY);
	if(fileDesc == -1){
		printf("open fifo: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	// Clearing str_message
	memset(str_message,0,sizeof(str_message));
	strcpy(str_message,"Test message");
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
								"Main write FIFO");
	if(write(fileDesc,str_message,sizeof(str_message))==-1){
		printf("write fifo: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	pthread_join(threadID, NULL);
	if(unlink(NAMEFIFO)==-1){
		printf("unlink: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	printf("Done!\n");
	return EXIT_SUCCESS;
}

void * secondThread(void * arg){
	char buffer[25];
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
									"Thread open FIFO");
	int fileDesc = open(NAMEFIFO, O_RDONLY);
	if(fileDesc == -1){
		printf("thread open: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
										"Thread read FIFO");
	if(read(fileDesc,buffer,13)==-1){
		printf("thread open: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("Message in thread: %s\n", buffer);
	pthread_exit(EXIT_SUCCESS);
}
