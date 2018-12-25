#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/trace.h>

#define MYCODE 21
#define FILEPATH "/net/QNX/home/pid"

int flag = 1;

void handlerSIGUSR1(int sig);

int main(int argc, char *argv[]) {
	pid_t 	PID = getpid();
	char str_message [45];
	char comForRun [128];
	if(argc < 2){
		printf("Missing argument (Node).\n");
		return EXIT_FAILURE;
	}
	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// --------------------------------
	sprintf(comForRun,"on -f /net/%s ./signal_send_net.run QNX &",argv[1]);
	signal(SIGUSR1,handlerSIGUSR1);
	sprintf(str_message,
			"Program sem_receive_net.c: PID %i",PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);

	// Write process PID
	FILE *filePid = fopen(FILEPATH,"w+");
	fprintf(filePid,"%i",PID);
	fclose(filePid);

	// Starting second program at another QNX machine
	system(comForRun);

	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
						"signal_receive_net.c waiting SIGUSR1");
	printf("Waiting!\n");
	while(flag);
	printf("Done!\n");
	return EXIT_SUCCESS;
}
void handlerSIGUSR1(int sig){
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
							"signal_receive_net.c Catch SIGUSR1");
	flag = 0;
}
