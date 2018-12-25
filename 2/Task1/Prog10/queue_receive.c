#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/trace.h>

#define MYCODE 21
#define NAMEQUEUE "MyQueue"

int main(int argc, char *argv[]) {
	pid_t 	PID = getpid();
	mqd_t mes;
	char str_message [45];
	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// -----------------
	sprintf(str_message,
			"Program queue_receive.c: PID %i",PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);
	// Creating queue
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
						"queue_receive open QUEUE");
	mes = mq_open(NAMEQUEUE, O_RDONLY | O_CREAT,
			S_IRWXU | S_IRWXG | S_IRWXO, NULL);
	if(mes == -1){
		printf("mq_open: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	// Starting second program
	system("./queue_send.run &");

	memset(str_message,0,sizeof(str_message));
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
						"queue_receive read QUEUE");
	if(mq_receive(mes, str_message, 4096, NULL)==-1){
		printf("mq_receive: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	printf("Incoming message: %s\n",str_message);
	if(mq_unlink(NAMEQUEUE)==-1){
		printf("mq_unlink: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	printf("Done!\n");
	return EXIT_SUCCESS;
}

