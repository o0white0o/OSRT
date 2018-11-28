//spawner
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <spawn.h>
#include <sched.h>
#include <errno.h>

int main(int argc,char *argv[]){
  if(argc<1)
    return -1;
  int count = atoi(argv[1]); //get count of spawned processes
  printf("Spawning %d processes\n",count);
  int i;
  pid_t pa[20]; //array of child PID
  char *arglist[] = {"ex3_proc","0",NULL}; //arguments list
  //spawning
  for(i=0;i<count;i++){
    //alter argument
    char buf[10];
    itoa(i+1,buf,10);
    arglist[1] = buf;
    //create child process with given argument
    pa[i] = spawnv(P_NOWAIT,"child",arglist);
    if(pa[i]<0)
      printf("Error: %s\n",strerror(errno));
    //change scheduling policy
    struct sched_param param;
    sched_getparam(pa[i],&param);
    //reduce priority
    param.sched_priority = 9;
    int res = sched_setscheduler(pa[i],SCHED_FIFO,&param);
    if(res <0)
      printf("Failed to set scheduler %s\n",strerror(errno));
    else
      printf("Spawned %d\n",i+1);

  }
  //info

  system("pidin | grep \"pid tid\"");
  system("pidin | grep child");

  //wait for children
  int status=-1; //return code of child
  for(i=0;i<count;i++){
    waitpid(pa[i],&status,0);
  }
  printf("Done!\n");
  return 0;
}
