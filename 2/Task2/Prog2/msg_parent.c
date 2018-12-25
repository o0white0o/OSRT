#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/trace.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>

#define MYCODE 21

int main(int argc, char *argv[]) {
	pid_t 	childPID,PID = getpid();
	char str_message [45];
	int chanId, coid;
	char buffer[80];
//	char comForRun [128];
//	if(argc < 2){
//		printf("Missing argument (Node).\n");
//		return EXIT_FAILURE;
//	}
	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// --------------------------------
//	sprintf(comForRun,"on -f /net/%s ./signal_send_net.run QNX &",argv[1]);
//	signal(SIGUSR1,handlerSIGUSR1);
	sprintf(str_message,
			"Program msg_parent.c (parent): PID %i",PID);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Parent create channel");
	chanId = ChannelCreate(0);
	if(chanId == -1){
		printf("ChannelCreate: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	childPID = fork();
	if(childPID == -1){
		printf("fork: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	if(childPID == 0){
		// Child code
		// Clearing str_message
		memset(str_message,0,sizeof(str_message));
		childPID = getpid();
		sprintf(str_message,
					"Program msg_parent.c (son): PID %i",childPID);
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
			printf("%s\n",str_message);
		memset(str_message,0,sizeof(str_message));
		strcpy(str_message,"Message for parent!");
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Child connect to channel");
		coid = ConnectAttach(ND_LOCAL_NODE, getppid(),chanId,_NTO_SIDE_CHANNEL,0);
		if(coid == -1){
			printf("ConnectAttach child: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		// Send message
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Child send message");
		if(MsgSend(coid,str_message,sizeof(str_message),
				buffer, sizeof(buffer)) == -1){
			printf("MsgSend: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Child receive reply");
		printf("Reply parent: %s\n",buffer);
		return EXIT_SUCCESS;
	}else{
		// Parent code
		int rcvId;
		memset(str_message,0,sizeof(str_message));
		strcpy(str_message,"Message for child!");
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
							"Parent connect to channel");
		coid = ConnectAttach(ND_LOCAL_NODE, 0, chanId,_NTO_SIDE_CHANNEL,0);
		if(coid == -1){
			printf("ConnectAttach parent: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		// Receive message from child
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
									"Parent receive message");
		rcvId = MsgReceive(chanId,buffer,sizeof(buffer),NULL);
		if(rcvId == -1){
			printf("MsgReceive: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		// Send reply to child
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
									"Parent send reply to child");
		if(MsgReply(rcvId,0,str_message, sizeof(str_message)) == -1){
			printf("MsgReceive: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		printf("Message from child: %s\n",buffer);
		wait(NULL);
		printf("Done!\n");
		return EXIT_SUCCESS;
	}
}
