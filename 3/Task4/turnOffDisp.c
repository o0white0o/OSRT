#include <errno.h>
#include <gf/gf.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
#include <sys/trace.h>

#define MYCODE 21
#define PERIODIC_TIMER 1
#define SINGLE_TIMER 2

struct itimerspec initTimeInterval(int mode, time_t second);
const struct sigevent* keyboardHandler (void *aread, int id);
struct sigevent event;
int main(int argc, char * argv[]){
    gf_display_t dispID;
    gf_dev_t gfx_dev;
    sigset_t sigset;
    timer_t timerID;
    struct itimerspec intTime;
    int result, iter;
    // Configure event to send signal SIGUSR1 after ready timer
    SIGEV_SIGNAL_INIT(&event, SIGUSR1);
    // Set periodic timer
	intTime = initTimeInterval(PERIODIC_TIMER,1);
	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// --------------------------------

    // Create timer
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Create timer");
    if(timer_create(CLOCK_REALTIME, &event, &timerID) == -1){
		printf("timer_create: %s\n", strerror(errno));
		return EXIT_FAILURE;
    }
    // Configure sigset
    sigaddset(&sigset, SIGUSR1);
    printf("Monitor turn-off after 5 sec. To cancel after monitor off, please press Esc...\n"
    		"Time left\n5\r");
    // Start relative periodic timer which wait for turn-off monitor
    TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Start relative periodic timer");
    if(timer_settime(timerID, 0, &intTime, NULL) == -1){
		printf("timer_settime: %s\n", strerror(errno));
		return EXIT_FAILURE;
    }
    for(iter = 4; iter>=0; iter--) {
    	// Wait signal from timer
    	sigwait(&sigset, &result);
        printf("%i\r", iter);
        fflush(stdout);
    }
    printf("\n");
    // Get descriptor video adapter
    TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Get descriptor video adapter");
	if(gf_dev_attach(&gfx_dev, GF_DEVICE_INDEX(0), NULL) != GF_ERR_OK){
		printf("gf_dev_attach: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	// Get descriptor the display
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Get descriptor the display");
	if(gf_display_attach(&dispID, gfx_dev, 0, NULL) != GF_ERR_OK){
		printf("gf_display_attach: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	// Turn-off display
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Turn-off display");
	if(gf_display_set_dpms(dispID, GF_DPMS_OFF) != GF_ERR_OK){
		printf("gf_display_set_dpms: %s\n", strerror(errno));
		printf("Maybe it is virtual machine?\n");
	}

    // Start absolute single timer which wait for turn-on monitor
	intTime = initTimeInterval(SINGLE_TIMER,6 + time(0));
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Start absolute single timer");
    if(timer_settime(timerID, TIMER_ABSTIME, &intTime, NULL) == -1){
		printf("timer_settime: %s\n", strerror(errno));
		return EXIT_FAILURE;
    }
    // Attach keyboard interrupt handler
    InterruptAttach(1, keyboardHandler, NULL, 0 , 0 | _NTO_INTR_FLAGS_TRK_MSK);
    // Wait signal from timer
    sigwait(&sigset, &result);

    // Turn-on display
    TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Turn-on display");
    if(gf_display_set_dpms(dispID, GF_DPMS_ON) != GF_ERR_OK){
		printf("gf_display_set_dpms: %s\n", strerror(errno));
		printf("Maybe it is virtual machine?\n");
    }
    printf("Done!\n");
    return EXIT_SUCCESS;
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
        return &event;
    }
    else
    	return NULL;
}
