#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>

int main(int argc,char *argv[]){
  volatile long i=0;
  //get start time
  struct timespec t_start, t_end;
  clock_gettime(CLOCK_REALTIME,&t_start);
  for(i=1;i<100000000UL;i++); //loop for some time
  //get end time
  clock_gettime(CLOCK_REALTIME,&t_end);
  float elapsed = (t_end.tv_sec-t_start.tv_sec)*1000+
      (float)(t_end.tv_nsec-t_start.tv_nsec)/1.0e6;
  char *policy;
  switch(sched_getscheduler(getpid())){
  case SCHED_FIFO:
    policy = "FIFO";
    break;
  case SCHED_RR:
    policy = "RR";
    break;
  case SCHED_OTHER:
    policy = "OTHER";
    break;
  case SCHED_SPORADIC:
    policy = "SPORADIC";
    break;
  }
  printf("Process %2s done with prio %d, elapsed %3.3f ms, policy %s\n",
         argv[1],getprio(getpid()),elapsed,policy);
  return 0;
}
