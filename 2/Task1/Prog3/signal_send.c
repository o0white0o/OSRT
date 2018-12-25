#include <stdlib.h>
#include <stdio.h>
#include <sys/trace.h>
#include <signal.h>

#define MYCODE 21

int main(int argc, char *argv[]) {
	pid_t pidReceiveProg;
	char str_message [45];
	sprintf(str_message,
			"Program signal_send.c: PID %i",getpid());
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);
	// Read PID receive program to file PID.txt
	FILE* file = fopen("PID.txt","r");
	fscanf(file,"%i",&pidReceiveProg);
	fclose(file);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
			"Send SIGUSR 1 to signal_rec");
	kill(pidReceiveProg, SIGUSR1);
	printf("Close send program!\n");
	return EXIT_SUCCESS;
}
