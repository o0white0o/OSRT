#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/trace.h>
#include <errno.h>

#define MYCODE 21
#define FIlEPATHNET "/net/QNX/tmp/transfer.txt"
#define FILEPATHLOCAL "/tmp/transfer.txt"

int main (int argc, char * argv[]) {
	pid_t PID = getpid();
	char str_message [45];
	char filePath [128];
	char readBuff[128];
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

	sprintf(str_message, "Program file_server.c: PID %i\n", PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Open file");
	fileDesc = open(filePath, O_RDONLY);
	if(fileDesc == -1){
		printf("Server: open: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Read message");
	if(read(fileDesc,&readBuff,sizeof(readBuff)) == -1){
		printf("Server: read: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Close file");
	close(fileDesc);
	printf("Server: Receiving message: %s\n", readBuff);
	printf("Server: Done!\n");
	return EXIT_SUCCESS;
}
