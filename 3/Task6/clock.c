#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/neutrino.h>
#include <unistd.h>
#include <sys/trace.h>

#define MYCODE 21
#define STEP_PERIOD 1000

// First experiment function
void ClockResolution(void);
// Second experiment function
void ChangeClockPeriod(void);
// Third experiment function
void CurrentTime(void);
// Fourth experiment function
void SmoothAdjustmentTime(void);

int main(int argc, char * argv[]){
	// Configure trace
	TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	TraceEvent(_NTO_TRACE_FLUSHBUFFER);
	// --------------------------------
	printf("--------- First experiment ---------\n");
	ClockResolution();
	printf("\n--------- Second experiment ---------\n\n");
	ChangeClockPeriod();
	printf("\n--------- Third experiment ---------\n\n");
	CurrentTime();
	printf("\n--------- Fourth experiment ---------\n\n");
	SmoothAdjustmentTime();
	printf("--------- ----------------- ---------\n");
	printf("Done!\n");
    return EXIT_SUCCESS;
}

void ClockResolution(void){
	struct timespec resolutClock;
	struct _clockperiod periodClock;
	printf("Find resolution of clock\n");
	// Get clock period
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Get clock period (clock_getres)");
	if(clock_getres(CLOCK_REALTIME, &resolutClock) == -1){
		printf("clock_getres: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("Resolution for CLOCK_REALTIME (clock_getres): %d ns\n", resolutClock.tv_nsec);
	// Get clock period
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Get clock period (ClockPeriod)");
	if(ClockPeriod(CLOCK_REALTIME, NULL, &periodClock, 0) == -1){
		printf("ClockPeriod: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("Resolution for CLOCK_REALTIME (ClockPeriod): %d ns\n", periodClock.nsec);
	return;
}

void ChangeClockPeriod(void){
	struct _clockperiod tempClockPeriod, defaultClockPeriod;
	printf("Acceptable limits change clock period\n");
	// Get clock period
	if(ClockPeriod(CLOCK_REALTIME, NULL, &defaultClockPeriod, NULL) == -1){
		printf("ClockPeriod: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf( "Current clock period is %d ns.\n", defaultClockPeriod.nsec );
	// Remember default clock period
	tempClockPeriod = defaultClockPeriod;
	printf("Decrement clock period\n");
	// Find minimum working clock period
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Set clock period (ClockPeriod)");
	while(ClockPeriod(CLOCK_REALTIME, &tempClockPeriod, NULL, NULL) == 0) {
		tempClockPeriod.nsec -= STEP_PERIOD;
	}
	tempClockPeriod.nsec += STEP_PERIOD;
	printf("Minimum working clock period is %d ns\n", tempClockPeriod.nsec);
	tempClockPeriod = defaultClockPeriod;
	printf("Increment clock period\n");
	// Find maximum working clock period
	while(ClockPeriod(CLOCK_REALTIME, &tempClockPeriod, NULL, NULL) == 0) {
		tempClockPeriod.nsec += STEP_PERIOD;
	}
	tempClockPeriod.nsec -= STEP_PERIOD;
	printf("Maximum working clock period is %d ns\n", tempClockPeriod.nsec);
	// Restore default clock period
	if(ClockPeriod(CLOCK_REALTIME, &defaultClockPeriod, NULL, NULL) == -1){
		printf("ClockPeriod: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	return;
}

void CurrentTime(void){
	struct timespec currTimeUNIX;
	time_t curTimeHuman;
	printf("Find and change current time.\n");
	// Get current time at UNIX format
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Get time (clock_gettime)");
	if(clock_gettime(CLOCK_REALTIME,&currTimeUNIX) == -1){
		printf("clock_gettime: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("Time at UNIX format: %d s %d ns\n",
			currTimeUNIX.tv_sec,currTimeUNIX.tv_nsec);
	// Get time at different format
	time(&curTimeHuman);
	printf("Time at human format: %s",
				ctime(&curTimeHuman));
	// Add 3 hours 30 minutes to current time
	printf("Add 3 hours 30 minutes to current time\n");
	currTimeUNIX.tv_sec += 3*60*60 + 30*60;
	// Set current time at UNIX format
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Set time (clock_settime)");
	if(clock_settime(CLOCK_REALTIME,&currTimeUNIX) == -1){
		printf("clock_settime: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	// Get current time at UNIX format
	if(clock_gettime(CLOCK_REALTIME,&currTimeUNIX) == -1){
		printf("clock_gettime: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("Time at UNIX format: %d s %d ns\n",
			currTimeUNIX.tv_sec,currTimeUNIX.tv_nsec);
	// Get time at different format
	time(&curTimeHuman);
	printf("Time at human format: %s",
				ctime(&curTimeHuman));
	// Removed 3 hours 30 minutes to current time
	printf("Removed 3 hours 30 minutes to current time\n");
	currTimeUNIX.tv_sec -= 4*60*60 - 30*60;
	// Set current time at UNIX format
	if(clock_settime(CLOCK_REALTIME,&currTimeUNIX) == -1){
		printf("clock_settime: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	// Get current time at UNIX format
	if(clock_gettime(CLOCK_REALTIME,&currTimeUNIX) == -1){
		printf("clock_gettime: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("Time at UNIX format: %d s %d ns\n",
			currTimeUNIX.tv_sec,currTimeUNIX.tv_nsec);
	// Get time at different format
	time(&curTimeHuman);
	printf("Time at human format: %s",
				ctime(&curTimeHuman));
}

void SmoothAdjustmentTime(void){
	time_t currentTime;
	struct _clockadjust shiftTime;
	int iter;
	printf("Smooth adjustment time.\n");
	// Get time at different format
	time(&currentTime);
	printf("Time at human format: %s",
				ctime(&currentTime));
	// How long shift time (4 seconds=4000 of clock periods (1 ms))
	shiftTime.tick_count = 4000;
	// Correct time (boost up to 0.5 ms every 1 ms)
	shiftTime.tick_nsec_inc = +500*1000;
	printf("Boost up current time to 0.5 ms every 1 ms\n");
	TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, MYCODE, "Boost up current time to 0.5 ms every 1 ms");
	if(ClockAdjust(CLOCK_REALTIME, &shiftTime, NULL) == -1){
		printf("ClockAdjust: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	for (iter=0; iter<4; iter++){
		// Get time at different format
		time(&currentTime);
		printf("Time at human format: %s",
				ctime(&currentTime));
		sleep(1);
	}
	// Correct time (slow up to 0.5 ms every 1 ms)
	shiftTime.tick_nsec_inc = -500*1000;
	printf("Slow up current time to 0.5 ms every 1 ms\n");
	if(ClockAdjust(CLOCK_REALTIME, &shiftTime, NULL) == -1){
		printf("ClockAdjust: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	for (iter=0; iter<4; iter++){
		// Get time at different format
		time(&currentTime);
		printf("Time at human format: %s",
				ctime(&currentTime));
		sleep(1);
	}
	return;
}


