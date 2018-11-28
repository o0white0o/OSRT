#include <stdio.h>
#include <sched.h>
int main() {
  printf("Priorities:\n");
  printf("SCHED_FIFO: min=%d\n",sched_get_priority_min(SCHED_FIFO));
  printf("SCHED_FIFO: max=%d\n",sched_get_priority_max(SCHED_FIFO));
  printf("SCHED_RR: min=%d\n",sched_get_priority_min(SCHED_RR));
  printf("SCHED_RR: max=%d\n",sched_get_priority_max(SCHED_RR));
  printf("SCHED_SPORADIC: min=%d\n",sched_get_priority_min(SCHED_SPORADIC));
  printf("SCHED_SPORADIC: max=%d\n",sched_get_priority_max(SCHED_SPORADIC));
  printf("SCHED_OTHER: min=%d\n",sched_get_priority_min(SCHED_OTHER));
  printf("SCHED_OTHER: max=%d\n",sched_get_priority_max(SCHED_OTHER));
  return 0;
}
