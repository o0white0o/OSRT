#include <stdio.h>
#include <time.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/trace.h>

#define MYCODE 21
#define PERIODIC_TIMER 1
#define SINGLE_TIMER 2

typedef struct{
	struct _pulse pulseTimer;
}msg_pulse_t;

struct itimerspec initTimeInterval(int mode, time_t second);
void printCurrentTime(char *str);

int main(int argc, char *argv[]) {
	struct sigevent event;
	struct itimerspec intTime;
	timer_t timerID;
	int channelId;
	msg_pulse_t msgPulse;
	int coid;

	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// --------------------------------

	// Create channel
	channelId = ChannelCreate(0);
	if(channelId == -1){
		printf("ChannelCreate: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	coid = ConnectAttach(ND_LOCAL_NODE,0,channelId,_NTO_SIDE_CHANNEL,0);
	if(coid == -1){
		printf("ConnectAttach: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	// Initialization sigevent for send pulse with code 24 and value 15
	SIGEV_PULSE_INIT(&event,coid,-1,24,15);
	// Create timer
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Create timer");
	if(timer_create(CLOCK_REALTIME, &event, &timerID) == -1){
		printf("timer_create: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	// Set single timer
	intTime = initTimeInterval(SINGLE_TIMER,1);
	printCurrentTime("Start relative single timer (1s):");
	// Start single relative timer
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Start relative single timer");
	if(timer_settime(timerID, 0, &intTime, NULL) == -1){
		printf("timer_settime: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	// Waiting pulse
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Waiting pulse");
	if(MsgReceivePulse(channelId, &msgPulse, sizeof(msgPulse), NULL) == -1){
		printf("MsgReceivePulse: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Receive pulse");
	printCurrentTime("End relative single timer:");
	printf("Receive pulse, code %d, value %d\n",
			msgPulse.pulseTimer.code, msgPulse.pulseTimer.value.sival_int);
	// Delete timer
	if(timer_delete(timerID) == -1){
		printf("timer_delete: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	printf("Done!\n");
	return 0;
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
