#include <stdlib.h>
#include <stdio.h>
#include <sys/trace.h>
#include <signal.h>

#define MYCODE 13

int stop = 0;

void signalHandlerUSR1(int signo);

int main(int argc, char *argv[]) {
	pid_t PID = getpid();
	char str_message [45];
	signal(SIGUSR1,signalHandlerUSR1);
	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// -----------------
	sprintf(str_message,
			"Program signal_rec.c: PID %i",PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);
	// Write current PID to file PID.txt
	FILE* file = fopen("PID.txt","w+");
	fprintf(file,"%i",PID);
	fclose(file);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
						"Waiting signal SIGUSR 1");
	for(;;){
		if(stop != 0)
			break;
	}

	printf("Done!\n");
	return EXIT_SUCCESS;
}

void signalHandlerUSR1(int signo){
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
			"Signal 1 handler enter");
	stop = 1;
}
