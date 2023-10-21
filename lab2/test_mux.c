/*
TEST mutex
*/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>  // Thread, Mutex

void *Thread_IN(void *arg);
void *Thread_OUT(void *arg);

char thread1[]="Thread A";
char thread2[]="Thread B";

pthread_mutex_t mutx; //create the mutex "mutx"

int counter = 0;  // Global variable
int queue = 0;

int main(int argc, char **argv)
{
  pthread_t t1, t2; //create the thread names t1 and t2
  void *thread_result;
  int state;

  // mutex initializtion
  state = pthread_mutex_init(&mutx, NULL);
  if(state)
  {
    puts("Error mutex initialization");
  }

  // create two threads
  pthread_create(&t1, NULL, Thread_IN, &thread1);
  pthread_create(&t2, NULL, Thread_OUT, &thread2);

  // waiting for thread1 to terminate
  pthread_join(t1, &thread_result);
  // waiting for thread2 to terminate
  pthread_join(t2, &thread_result);
  
  printf("Terminated %s, %s !!!\n", &thread1, &thread2);
  printf("Total Counts for threads: %d\n", counter);

  pthread_mutex_destroy(&mutx);	//destory mutex
  return 0;
}

// Each Thread increases number
void *Thread_IN(void *arg)
{
  int i;
  printf("Creating Thread: %s\n", (char*)arg);
  for(i=0; i<2; i++)	// critical section
  {
    pthread_mutex_lock(&mutx);	// mutex lock before critical section 
    sleep(1);	// sleep few seconds to show for user
    
    queue++;
    counter++;
    printf("%s: INSERT item to BUFFER %d\n", (char*)arg, queue);
    pthread_mutex_unlock(&mutx);	// mutex unlock
  }
}

void *Thread_OUT(void *arg)
{
  int i;
  printf("Creating Thread: %s\n", (char*)arg);
  for(i=0; i<2; i++)	// critical section
  {
    pthread_mutex_lock(&mutx);	// mutex lock before critical section 
    sleep(1);	// sleep few seconds to show for user

    printf("%s: REMOVE item from BUFFER %d\n", (char*)arg, queue);
    
    queue--;
    counter++;
    pthread_mutex_unlock(&mutx);	// mutex unlock
  }
}
