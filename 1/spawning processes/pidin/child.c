#include <stdio.h>
#include <unistd.h>

int main(int argc,char *argv[]){
  printf("Process %s started, pid %d, ppid %d\n",argv[1],getpid(),getppid());
  int count=0;
  sleep(3);
  return 0;
}
