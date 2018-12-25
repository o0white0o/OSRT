#include <stdlib.h>
#include <stdio.h>
#include <sys/trace.h>
#include <signal.h>

#define MYCODE 13

int 		stop = 0;

void signalHandlerUSR1(int signo);

void signalHandlerUSR2(int signo);

int main(int argc, char *argv[]) {
	pid_t sonPid;
	pid_t parentPid = getpid();
	char str_message [45];
	struct sigaction action;
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set,SIGUSR2);
	action.sa_handler = &signalHandlerUSR2;
	action.sa_flags = 0;
	action.sa_mask = set;
	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// -----------------
	sprintf(str_message,
			"Prog. signal2.c: parent PID %i",parentPid);
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, str_message);
	printf("%s\n",str_message);
	sonPid = fork();
	if(sonPid == -1)
		exit(EXIT_FAILURE);
	if(sonPid == 0){
		// Son code
		signal(SIGUSR1, signalHandlerUSR1);
		sigaction(SIGUSR2, &action, NULL);
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
				"Prog. signal2.c: Son work");
		// Waiting SIGUSR1
		for(;;){
			if(stop != 0)
				break;
		}
		stop = 0;
		// Waiting SIGUSR2
		for(;;){
			if(stop != 0)
				break;
		}
		return EXIT_SUCCESS;
	}else{
		// Father code
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
						"Prog. signal2.c: Father work");
		printf("Prog. signal2.c: son PID %i\n",sonPid);
		sleep(0.5);
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
						"Send SIGUSR1 to son");
		kill(sonPid,SIGUSR1);
		sleep(0.5);
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
						"Send SIGUSR2 to son");
		kill(sonPid,SIGUSR2);
		wait(NULL);
		printf("Done!\n");
		return EXIT_SUCCESS;
	}
}

void signalHandlerUSR1(int signo){
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
			"Signal 1 handler enter");
	stop = 1;
}

void signalHandlerUSR2(int signo){
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
			"Signal 2 handler enter");
	if(stop == 0){
		kill(getpid(),SIGUSR2);
		kill(getpid(),SIGUSR1);
	}
	for(;;){
		if(stop != 0)
			break;
	}
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE,
			"Signal 2 handler exit");
}
