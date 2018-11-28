#include <stdio.h>
#include <unistd.h>
int main(int argc,char *argv[]){
  usleep(100000);
  long count = 50000000;
  while (count>0) {
    count --;
  }
  printf("Process %s done\n",argv[1]);
  return 0;
}
