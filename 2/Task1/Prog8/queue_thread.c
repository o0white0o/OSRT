#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/trace.h>

#define MYCODE 21
#define NAMEQUEUE "MyQueue"

pthread_t threadID;
mqd_t mes;

void * secondThread(void * arg);

int main(int argc, char *argv[]) {
	pid_t 	PID = getpid();
	char str_message [45];
	/*char comForRun [128];
	if(argc <2){
		printf("Missing arguments (Node).\n");
		return EXIT_FAILURE;
	}

	// Preparing net starting
	sprintf(comForRun, "on -f /net/%s ./fifo_send.run &",
			argv[1]);
	*/
	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// -----------------
	sprintf(str_message,
			"Program queue_thread.c: PID %i",PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);

	system("ls -l");
	// Creating queue
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
							"Open queue");
	mes = mq_open(NAMEQUEUE,O_RDWR | O_CREAT,
			S_IRWXU | S_IRWXG | S_IRWXO, NULL);
	if(mes==-1){
		printf("mq_open: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	system("ls -l");

	// Starting new thread
	pthread_create(&threadID,NULL,secondThread,NULL);


	// Read message
	memset(str_message,0,sizeof(str_message));
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
						"Main thread read queue");
	if(mq_receive(mes,str_message,4096,NULL)==-1){
		printf("mq_receive: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	printf("Incoming message: %s\n",str_message);
	pthread_join(threadID,NULL);
	if(mq_unlink(NAMEQUEUE)==-1){
		printf("unlink: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	printf("Done!\n");
	return EXIT_SUCCESS;
}

void * secondThread(void * arg){
	char message [] = "Test message";
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE + 2,
							"Second thread write queue");
	if(mq_send(mes, message, sizeof(message), 0)==-1) {
		printf( "mq_send: %s\n", strerror(errno));
	}
	pthread_exit(NULL);
}
