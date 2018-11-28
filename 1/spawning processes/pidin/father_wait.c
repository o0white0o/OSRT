#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
int main(void){
  printf("Father pid %d,spawning\n",getpid());
  int i;
  char *argv[] = {"0","0",NULL};
  pid_t pid_array[5];
  for(i=0;i<5;i++)
  {
    char buf[10];
    itoa(i+1,buf,10);
    argv[1] =  buf;
    pid_array[i] = spawnv(P_NOWAIT,"child",argv);
    printf("Spawned child, got its pid %d\n",(int)pid_array[i]);
  }
  //display proc info
  system("pidin | grep \"pid tid\"");
  system("pidin | grep child");
  int status=-1;
  printf("Parent waiting\n");
  for(i=0;i<5;i++)
  {
    waitpid(pid_array[i],&status,0);
    printf("Child pid %d finished, code %d\n",(int)pid_array[i],status);
  }
  printf("Done\n");
  return 0;
}
