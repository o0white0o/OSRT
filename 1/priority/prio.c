#include <stdio.h>
main () {
  int prio;
  int pid;
  pid = getpid();
  prio=getprio(pid);
  printf("Priority is %d\n", prio);
  setprio(pid,9);
  prio=getprio(pid);
  printf("Set priority is 9\n");
  printf("New priority is %d\n",prio);
  setprio(pid,9999);
  prio=getprio(pid);
  printf("Set priority is 9999\n");
  printf("New priority is %d\n",prio);
  setprio(pid,1);
  prio=getprio(pid);
  printf("Set priority is 1\n");
  printf("New priority is %d\n",prio);
  setprio(pid,1);
  prio=getprio(pid);
  printf("Set priority is 1\n");
  printf("New priority is %d\n",prio);
  setprio(pid,-1000);
  prio=getprio(pid);
  printf("Set priority is -1000\n");
  printf("New priority is %d\n",prio);
}
