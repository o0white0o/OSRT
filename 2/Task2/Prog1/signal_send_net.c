#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/trace.h>

#define MYCODE 23
#define FILEPATH "/net/QNX/home/pid"

int main(int argc, char *argv[]) {
	pid_t 	PID = getpid();
	int rec_PID;
	char str_message [45];

	int recNodeDesc;
	if(argc < 2){
		printf("Missing argument (Node).\n");
		return EXIT_FAILURE;
	}
	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// --------------------------------

	sprintf(str_message,
			"Program sem_send_net.c: PID %i",PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);

	// Convert a string to node descriptor
	recNodeDesc = netmgr_strtond(argv[1], NULL);
	if(recNodeDesc == -1){
		printf("sem_open: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	// Read process PID
	FILE *filePid = fopen(FILEPATH,"r");
	fscanf(filePid,"%i",&rec_PID);
	fclose(filePid);

	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
						"signal_send_net.c send SIGUSR1");
	SignalKill(recNodeDesc,rec_PID,0,SIGUSR1,0,0);
	printf("Done!\n");
	return EXIT_SUCCESS;
}
