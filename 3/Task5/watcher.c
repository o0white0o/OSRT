#include <stdlib.h>
#include <stdio.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <sys/dispatch.h>
#include <sys/trace.h>
#include <errno.h>
#include <hw/inout.h>
#include <signal.h>

// Timer mode
#define PERIODIC_TIMER 1
#define SINGLE_TIMER 2
// Other
#define MYCODE 21
#define NAME_CHANNEL "Watcher"
#define SEND_DATA_TO_SERVER 1
#define END_WORK 2
#define CONNECT_SERVER 3
#define SERVER_AVAILABLE 4

// Structure for send message
typedef struct{
	struct _pulse pulseData;
	float data;
} message_t;

// Function for configure timer interval
struct itimerspec initTimeInterval(int mode, time_t second);
// Keyboard interrupt handler
const struct sigevent* keyboardHandler (void *aread, int id);
// Signal handler
void signalHandlerUSR1(int signo);
// Event for timer
struct sigevent event;
volatile int stopWatcher = 0;

int main(int argc, char *argv[]) {
	int resSend, result, coid, idInterKeyboard;
	sigset_t sigset;
	timer_t timerID;
	message_t msgSend, msgRec;
	struct itimerspec intTime;

	// Set name for main thread
	pthread_setname_np(pthread_self(),"Watcher");
	printf("Watcher started, PID %i\n",(int)getpid());
	printf("For stop monitoring, please press ESC.\n");
	// Configure event to send signal SIGUSR1 after ready timer
	SIGEV_SIGNAL_INIT(&event, SIGUSR1);
	// Set periodic timer
	intTime = initTimeInterval(PERIODIC_TIMER,2);
	// Attach handler for signal
	signal(SIGUSR1,signalHandlerUSR1);
	// Get super thread
	ThreadCtl(_NTO_TCTL_IO, 0);

	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// --------------------------------

	// Open channel
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Watcher open channel to server");
	coid = name_open(NAME_CHANNEL, NAME_FLAG_ATTACH_GLOBAL);

	// Create timer
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Create timer");
	if(timer_create(CLOCK_REALTIME, &event, &timerID) == -1){
		printf("timer_create: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	// Attach keyboard interrupt handler
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Attach interrupt");
	idInterKeyboard = InterruptAttach(1, &keyboardHandler, NULL, 0 , 0);
	if(idInterKeyboard == -1){
		printf("InterruptAttach: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	// Configure sigset
	sigaddset(&sigset, SIGUSR1);
	msgSend.pulseData.code = SEND_DATA_TO_SERVER;
	while(!stopWatcher){
		// Start relative periodic timer which wait timeout for SEND and REPLY block
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Start timeout timer");
		if(timer_settime(timerID, 0, &intTime, NULL) == -1){
			printf("timer_settime: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Send message to server");
		resSend = MsgSend(coid,&msgSend, sizeof(msgSend),
								&msgRec, sizeof(msgRec));
		if(resSend == CONNECT_SERVER){
			printf("Watcher connect (or reconnect) to server.\n");
		}
		if(resSend == SERVER_AVAILABLE){
			TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Node is available.");
			printf("Watcher: Node is available.\n");
		}else{
			TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Node is unavailable.");
			printf("Watcher: WARNING! Node is unavailable.\n");
			// Try reconnect
			name_close(coid);
			coid = name_open(NAME_CHANNEL, NAME_FLAG_ATTACH_GLOBAL);
		}
		// Start relative periodic timer which wait for some time
		TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Start wait timer");
		if(timer_settime(timerID, 0, &intTime, NULL) == -1){
			printf("timer_settime: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		// Wait signal from timer
		sigwait(&sigset, &result);
	}

	MsgSendPulse(coid,-1,END_WORK,0);
	InterruptDetach(idInterKeyboard);
	name_close(coid);
	printf("Done!\n");
	return EXIT_SUCCESS;
}

void signalHandlerUSR1(int signo){
	printf("Time to reply from server is out.\n");
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

const struct sigevent *keyboardHandler(void *area, int id) {
	// Read keyboard press
    int key=in8(0x60);
    // Press ESC?
    if (key==1) {
    	stopWatcher = 1;
        return &event;
    }
    else
    	return NULL;
}
