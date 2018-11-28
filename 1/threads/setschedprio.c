#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <unistd.h>

void *create_thread(void *arg) {
  int *number = (int *) arg;
  printf("Thread %d started\n", *number);
  int i;
  for (i = 0; i < 1000000000U; ++i);
  pthread_exit(0);
}

int main(int argc, char *argv[]) {
  if (argc < 2)
    return -1;
  int number_threads = atoi(argv[1]);
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_t threads[20];
  int i;
  int arg_array[20];
  for (i = 0; i < number_threads; ++i) {
    arg_array[i] = i + 1;
    pthread_create(&threads[i], &attr, &create_thread, (void *) &arg_array[i]);
    pthread_setschedprio(threads[i], getprio(getpid()) - i + 1);
  }
  system("pidin -F \"%n\t%a\t%b\t%p\t%T\t%J\" | grep \"/setschedprio\"");

  for (i = 1; i < number_threads; i++) {
    pthread_join(threads[i], NULL);
  }
  printf("All done\n");
  return 0;
}
