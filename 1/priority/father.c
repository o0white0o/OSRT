#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <spawn.h>

int main(int argc,char *argv[]){
  int i;
  pid_t pa[20];
  char *arglist[] = {"proc","0",NULL};
  //spawning
  for(i=0;i<5;i++){
    char buf[10];
    itoa(i+1,buf,10);
    arglist[1] = buf;
    pa[i] = spawnv(P_NOWAIT,"cycle",arglist);
  }
  system("pidin | grep \"pid tid\"");
  system("pidin | grep cycle");
  int status=-1;
  for(i=0;i<5;i++){
    waitpid(pa[i],&status,0);
  }
  printf("Done\n");
  return 0;
}
