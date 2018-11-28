#include <stdio.h>
#include <sched.h>
int main()
{
  struct sched_param param;
  int pr_prio=9;
  sched_getparam(getpid(), &param);
  printf("old priority: %d\n",param.sched_priority);
  param.sched_priority = pr_prio;
  sched_setparam(getpid(), &param);
  printf("new priority: %d\n",param.sched_priority);
  return 0;
}
