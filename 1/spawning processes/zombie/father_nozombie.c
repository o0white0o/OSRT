#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <spawn.h>

int main(int argc,char *argv[]){
  printf("Father pid %d,spawning\n",getpid());
  struct inheritance inherit;
  inherit.flags = SPAWN_NOZOMBIE;
  char *argv_child[] = {"0","0",NULL};
  pid_t pid_array[5];
  int i;
  for(i=0;i<5;i++)
  {
    char buf[10];
    itoa(i+1,buf,10);
    argv_child[1] =  buf;
    pid_array[i] = spawn("child",0,NULL,&inherit,argv,NULL);
    printf("spawned child, got its pid %d\n",(int)pid_array[i]);
  }
  sleep(2);
  system("pidin | grep \"pid tid \"");
  system("pidin | grep father");
  system("pidin | grep child");
  system("pidin | grep Zombie");
  sleep(2);
  printf("Parent waiting\n");
  int status = -1;
  waitpid(pid_array[0],&status,0);
  printf("Done\n");
  return 0;
}