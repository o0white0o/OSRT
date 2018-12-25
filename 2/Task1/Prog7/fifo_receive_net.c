#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/trace.h>

#define MYCODE 21
#define NAMEFIFO "MyFIFO"

int main(int argc, char *argv[]) {
	pid_t 	PID = getpid();
	char str_message [45];
	char comForRun [128];
	if(argc <2){
		printf("Missing arguments (Node).\n");
		return EXIT_FAILURE;
	}

	// Preparing net starting
	sprintf(comForRun, "on -f /net/%s ./fifo_send.run &",
			argv[1]);

	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// -----------------
	sprintf(str_message,
			"Program fifo_receive.c: PID %i",PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);

	// Creating fifo
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
							"Create FIFO");
	if(mkfifo(NAMEFIFO,S_IRWXU | S_IRWXG | S_IRWXO)==-1){
		printf("mkfifo: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	// Starting second program
	system(comForRun);

	// Open FIFO
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
					"Fifo_receive open FIFO");
	int fileDesc = open(NAMEFIFO, O_RDONLY);
	if(fileDesc == -1){
		printf("open fifo: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	// Read message
	memset(str_message,0,sizeof(str_message));
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
						"Fifo_receive read FIFO");
	if(read(fileDesc,str_message,13)==-1){
		printf("thread open: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	printf("Incoming message: %s\n",str_message);
	if(unlink(NAMEFIFO)==-1){
		printf("unlink: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	printf("Done!\n");
	return EXIT_SUCCESS;
}

