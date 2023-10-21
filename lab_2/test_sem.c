/*
TEST semaphore
Consider binary semaphore
*/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>

void *thread_put(void *arg);	// function for sending
void *thread_out(void *arg);	// function for receiving

sem_t bin_sem;	// semaphore

int number = 0;	// global variable, all threads can acess

char thread1[]="Thread A";
char thread2[]="Thread B";
char thread3[]="Thread C";

int main(int argc, char **argv)
{
  pthread_t t1, t2, t3;
  void *thread_result;
  int state;

  state = sem_init(&bin_sem, 0 ,0);
  //semaphore initialization, first value = 0

  if(state!=0)
    puts("Error semaphore initialization!!!");

  // Create thread1, thread2, thread3
  pthread_create(&t1, NULL, thread_put, &thread1);
  pthread_create(&t2, NULL, thread_out, &thread2);
  pthread_create(&t3, NULL, thread_out, &thread3);

  // Waiting thread to terminate
  pthread_join(t1, &thread_result);
  pthread_join(t2, &thread_result);
  pthread_join(t3, &thread_result);

  printf("Terminate => %s, %s, %s!!!\n", &thread1, &thread2, &thread3);
  printf("Final Index: %d\n", number);

  sem_destroy(&bin_sem);	// destroy semaphore
  return 0;
}

// Thread increases data to 1
void *thread_put(void *arg)
{
  int i;
  printf("Creating Thread: %s\n", (char*)arg);
  
  for(i=0; i<6; i++)
  {
    while(number!=0)
    {
      sleep(2);	// waiting until number = 0
    }

    number++;
    printf("PUT %s, Index: %d\n", (char*)arg, number);
    sem_post(&bin_sem);	// semaphore to 1
  }
}

// Thread decreases data to 0
void *thread_out(void *arg)
{
  int i;
  
  printf("Creating Thread: %s\n", (char*)arg);
  for(i=0;i<3;i++)
  {
    sem_wait(&bin_sem);	// semaphore to 0
    number--;
    printf("OUT %s, Index: %d\n", (char*)arg, number);
  }
}

