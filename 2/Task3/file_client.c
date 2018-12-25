#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/trace.h>
#include <errno.h>

#define MYCODE 23
#define FIlEPATHNET "/net/QNX/tmp/transfer.txt"
#define FILEPATHLOCAL "/tmp/transfer.txt"

int main (int argc, char * argv[]) {
	pid_t PID = getpid();
	char str_message [45];
	char filePath [128];
	char message[] = "Hello? It,s server?";
	int fileDesc;
	int iter = 0;
	if(argc < 2 ) {
		printf("Missing arguments! (type work -l - local, -n - network)\n");
		return EXIT_SUCCESS;
	}

	if(strcmp(argv[1],"-l") == 0)
		sprintf(filePath, FILEPATHLOCAL);
	else
		sprintf(filePath, FIlEPATHNET);

	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// --------------------------------

	sprintf(str_message, "Program file_client.c: PID %i",PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Open file");
	fileDesc = open(filePath, O_CREAT | O_WRONLY);
	if(fileDesc == -1){
		printf("Client: open: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Send message");
	if(write(fileDesc, message,sizeof(message)) == -1){
		printf("Client: write: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	printf("Client: Message send!\n");
	close(fileDesc);
	printf("Client: Done!\n");
	return EXIT_SUCCESS;
}
