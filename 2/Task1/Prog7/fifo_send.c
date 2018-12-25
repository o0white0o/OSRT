#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/trace.h>

#define MYCODE 23
#define NAMEFIFO "MyFIFO"

int main(int argc, char *argv[]) {
	pid_t 	PID = getpid();
	char str_message [45];
	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// -----------------
	sprintf(str_message,
			"Program fifo_send.c: PID %i",PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);
	// Open FIFO
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
								"Fifo_send open FIFO");
	int fileDesc = open(NAMEFIFO, O_WRONLY);
	if(fileDesc == -1){
		printf("open fifo: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	memset(str_message,0,sizeof(str_message));
	strcpy(str_message,"Test message");
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
						"Fifo_send write FIFO");
	if(write(fileDesc,str_message,sizeof(str_message))==-1){
		printf("write fifo: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
