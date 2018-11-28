#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc,char *argv[]){
  printf("Father pid %d,spawning\n",getpid());
  int is_wait = (int)argv[1][0] - (int)'0';
  int i;
  char *argv_child[] = {"0","0",NULL};
  pid_t pid_array[5];
  for(i=0;i<5;i++)
  {
    char buf[10];
    itoa(i+1,buf,10);
    argv_child[1] =  buf;
    pid_array[i] = spawnv(is_wait,"child",argv_child);
    printf("spawned child, got its pid %d\n",(int)pid_array[i]);
  }
  printf("Done\n");
  return 0;
}