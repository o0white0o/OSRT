#include <stdio.h>
#include <unistd.h>

int main(int argc,char *argv[]){
  printf("Process %s started, pid %d, ppid %d\n",argv[1],getpid(),getppid());
  sleep(2);
  return 0;
}
