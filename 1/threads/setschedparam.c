#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

//Thread function
void * f_thread(void *varg){
  int *arg = (int*)varg;
  int policy;
  struct sched_param param;
  usleep(10000);
  int res = pthread_getschedparam(pthread_self(),&policy,&param);
  char *pstr;
  switch(policy){
  case SCHED_FIFO:
    pstr = "FIFO";
    break;
  case SCHED_RR:
    pstr = "RR";
    break;
  case SCHED_OTHER:
    pstr = "OTHER";
    break;
  case SCHED_SPORADIC:
    pstr = "SPORADIC";
    break;
  }
  printf("Thread %d started, policy %s\n",*arg,pstr);
  int i;
  //loop for some time
  for(i=0;i<1000000000UL;i++);
  pthread_exit(0);
}

//Create N threads with given scheduling policy
int main(int argc,char *argv[]){
  if(argc < 2)
    return -1;
  //get threads count
  int n = atoi(argv[1]);
  //get policy type
  int pnum = atoi(argv[2]);
  //create thread attribute structure
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  //thread ID array
  pthread_t threads[20];
  int i;
  //thread arguments
  int arg_array[20];
  for(i=0;i<n;i++){
    arg_array[i] = i+1;
    pthread_create(&threads[i],&attr,&f_thread,(void*)&arg_array[i]);
    //change priority
    pthread_setschedprio(threads[i],getprio(getpid()));
    //change policy
    struct sched_param param;
    int policy;
    pthread_getschedparam(threads[i],&policy,&param);
    if (pnum == 1)
      policy = SCHED_FIFO;
    else  if (pnum == 2)
      policy = SCHED_RR;
    if(pthread_setschedparam(threads[i],policy,&param)!=EOK)
      strerror(errno);
  }

  //display thread info
  printf("NAME     PID           THREAD ID   PRIO    N THREADS  STATE\n");
  system("pidin -F \"%10n %10a %5b %5p %5T %10J\" | grep \"/setschedparam\"");

  //Join all threads
  for(i=1;i<n;i++)
  {
    pthread_join(threads[i],NULL);
  }
  printf("All done\n");
  return 0;
}
