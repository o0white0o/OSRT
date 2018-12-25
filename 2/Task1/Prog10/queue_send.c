#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/trace.h>

#define MYCODE 23
#define NAMEQUEUE "MyQueue"

int main(int argc, char *argv[]) {
	pid_t 	PID = getpid();
	char str_message [45];
	mqd_t mes;
	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// -----------------
	sprintf(str_message,
			"Program queue_send.c: PID %i",PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);
	// Open FIFO
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
								"queue_send open QUEUE");
	mes = mq_open(NAMEQUEUE, O_WRONLY,
			S_IRWXU | S_IRWXG | S_IRWXO, NULL);
	if(mes == -1){
		printf("mq_open: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	memset(str_message,0,sizeof(str_message));
	strcpy(str_message,"Test message");
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
						"queue_send write QUEUE");
	if(mq_send(mes, str_message, sizeof(str_message), 0)==-1){
		printf("mq_send: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
