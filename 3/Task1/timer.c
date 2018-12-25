#include <stdio.h>
#include <time.h>
#include <sys/netmgr.h>
#include <signal.h>
#include <sys/neutrino.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/trace.h>

#define MYCODE 21
#define PERIODIC_TIMER 1
#define SINGLE_TIMER 2

int countSignal = 0;

void signalHandler(int arg);
struct itimerspec initTimeInterval(int mode, time_t second);
void printCurrentTime(char *str);

int main(int argc, char *argv[]) {
	struct sigevent event;
	struct itimerspec intTime;
	timer_t timerID;

	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// --------------------------------

	// Initialization sigevent for send signal
	SIGEV_SIGNAL_INIT(&event,SIGUSR1);
	signal(SIGUSR1,signalHandler);
	// Create timer
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Create timer");
	if(timer_create(CLOCK_REALTIME, &event, &timerID) == -1){
		printf("timer_create: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	// Set serial timer
	intTime = initTimeInterval(SINGLE_TIMER,1);
	printCurrentTime("Start relative single timer (1s):");
	// Start single relative timer
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Start relative single timer");
	if(timer_settime(timerID, 0, &intTime, NULL) == -1){
		printf("timer_settime: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	// Waiting signal
	sleep(5);
	// Starting periodic timer
	intTime = initTimeInterval(PERIODIC_TIMER,2);
	printCurrentTime("\nStart relative periodic timer (2s):");
	// Start periodic relative timer
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Start relative periodic timer");
	if(timer_settime(timerID, 0, &intTime, NULL) == -1){
		printf("timer_settime: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	// Waiting 3 signal
	for(;;){
		if(countSignal <= 3)
			sleep(3);
		else
			break;
	}
	// Starting single timer
	intTime = initTimeInterval(SINGLE_TIMER,time(NULL)+3);
	printCurrentTime("\nStart absolute single timer (+3s):");
	// Start periodic absolute timer
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Start absolute single timer");
	if(timer_settime(timerID, TIMER_ABSTIME, &intTime, NULL) == -1){
		printf("timer_settime: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	// Waiting signal
	sleep(5);
	if(timer_delete(timerID) == -1){
		printf("timer_delete: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	printf("Done!\n");
	return 0;
}

void signalHandler(int arg){
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "End timer");
	if(!countSignal)
		printCurrentTime("End relative single timer:");
	else if(countSignal <= 3)
		printCurrentTime("End relative period timer:");
	else
		printCurrentTime("End absolute single timer:");
	++countSignal;
	return;
}

struct itimerspec initTimeInterval(int mode, time_t second){
	struct itimerspec intTime;
	switch(mode){
		case PERIODIC_TIMER:
			// Timer interval
			intTime.it_value.tv_sec = second;
			intTime.it_value.tv_nsec = 0;
			// Reload timer interval
			intTime.it_interval.tv_sec = second;
			intTime.it_interval.tv_nsec = 0;
			break;
		case SINGLE_TIMER:
			// Timer interval
			intTime.it_value.tv_sec = second;
			intTime.it_value.tv_nsec = 0;
			// Reload timer interval
			intTime.it_interval.tv_sec = 0;
			intTime.it_interval.tv_nsec = 0;
			break;
		default:
			// Timer interval
			intTime.it_value.tv_sec = 1;
			intTime.it_value.tv_nsec = 0;
			// Reload timer interval
			intTime.it_interval.tv_sec = 1;
			intTime.it_interval.tv_nsec = 0;
			break;
	}
	return intTime;
}

void printCurrentTime(char *str){
	time_t currentTime;
	char clockString[16];
	time(&currentTime);
	strftime(clockString, 16,"Time: %H:%M:%S",localtime(&currentTime));
	printf("%s %s\n",str,clockString);
	return;
}
