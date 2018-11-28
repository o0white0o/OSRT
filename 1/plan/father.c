#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <spawn.h>
#include <sched.h>
#include <errno.h>
#include <process.h>

int main(int argc, char *argv[]) {
  if (argc < 1)
    return -1;
  int count = atoi(argv[1]);
  if (argc != count + 3)
    return -1;
  int disp = atoi(argv[count + 2]);
  printf("Spawning %d processes\n", count);
  int i;
  pid_t pr_pid[20];
  char *arglist[] = {"id", "0", NULL};
  for (i = 0; i < count; i++) {
    char buf[10];
    itoa(i + 1, buf, 10);
    arglist[1] = buf;
    pr_pid[i] = spawnv(P_NOWAIT, "child", arglist);
    if (pr_pid[i] < 0) {
      printf("Error: %s\n", strerror(errno));
      continue;
    }
    struct sched_param param;
    sched_getparam(pr_pid[i],&param);
    param.sched_priority = atoi(argv[2 + i]);
    int res;
    switch (disp) {
    case 1: res = sched_setscheduler(pr_pid[i], SCHED_FIFO, &param);
      break;
    case 2: res = sched_setscheduler(pr_pid[i], SCHED_RR, &param);
      break;
    case 3: res = sched_setscheduler(pr_pid[i], SCHED_OTHER, &param);
      break;
    case 4: {
      struct timespec t;
      t.tv_sec = 0;
      t.tv_nsec = 20e6; //200 ms
      param.sched_ss_init_budget = t;
      //set T - repl period
      t.tv_sec = 0;
      t.tv_nsec = 200e6; //20 ms
      param.sched_ss_repl_period = t;
      //set low prioirity
      param.sched_ss_low_priority = 8;
      //set max replenishments number
      param.sched_ss_max_repl = 1000;
      res = sched_setscheduler(pr_pid[i], SCHED_SPORADIC, &param);
      break;
    }
    }
    if (res < 0)
      printf("Failed to set scheduler\n");
    else
      printf("Spawned %d\n", i + 1);

  }
  system("pidin | grep \"pid tid\"");
  system("pidin | child ");
  int status = -1;
  for (i = 0; i < count; i++) {
    waitpid(pr_pid[i], &status, 0);
  }
  printf("Done\n");
  return 0;
}
