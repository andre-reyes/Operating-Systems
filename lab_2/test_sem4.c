/*
TEST semaphore and mutex
Consider binary semaphore
*/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>
#include<signal.h>

#define BUFFER_SIZE 6
typedef int buffer_item;

buffer_item buffer[BUFFER_SIZE];

// global variable, all threads can acess
int index_counter = 0;

void *thread_Insert(void *arg);	// function for sending
void *thread_Remove(void *arg);	// function for receiving

sem_t bin_sem;	// semaphore
pthread_mutex_t mutx;	// mutex

char thread1[]="Thread A";
char thread2[]="Thread B";
char thread3[]="Thread C";

int main(int argc, char **argv)
{
  pthread_t t1, t2, t3;
  void *thread_result;
  int state1, state2;

  state1 = pthread_mutex_init(&mutx, NULL);
  state2 = sem_init(&bin_sem, 0 ,0);
  //mutex initialization
  //semaphore initialization, first value = 0

  if(state1||state2!=0)
    puts("Error mutex & semaphore initialization!!!");

  // Create thread1, thread2, thread3
  pthread_create(&t1, NULL, thread_Insert, &thread1);
  pthread_create(&t2, NULL, thread_Remove, &thread2);
  pthread_create(&t3, NULL, thread_Remove, &thread3);

  // Waiting thread to terminate
  pthread_join(t1, &thread_result);
  pthread_join(t2, &thread_result);
  pthread_join(t3, &thread_result);

  printf("Terminate => %s, %s, %s!!!\n", &thread1, &thread2, &thread3);
  printf("Final Index: %d\n", index_counter);

  sem_destroy(&bin_sem);	// destroy semaphore
  pthread_mutex_destroy(&mutx);	// destroy mutex
  
  return 0;
}

// Thread increases item
void *thread_Insert(void *arg)
{
  int i;
  printf("Creating Thread: %s\n", (char*)arg);
  
  for(i=0;i<BUFFER_SIZE;i++)
  {
    pthread_mutex_lock(&mutx);
    
    if(index_counter<BUFFER_SIZE)
    {
      buffer[index_counter] = index_counter;
      index_counter++;
      
      printf("%s: INSERT item to BUFFER %d\n", (char*)arg, index_counter);
      sem_post(&bin_sem);	// semaphore to increase
    }
    else
    {
      sleep(2);
    }
    pthread_mutex_unlock(&mutx);
  }
}

// Thread decreases item
void *thread_Remove(void *arg)
{
  int i;
  
  printf("Creating Thread: %s\n", (char*)arg);
  
  for(i=0;i<BUFFER_SIZE/2;i++)
  {
    sem_wait(&bin_sem);	//decrease index_counter
    pthread_mutex_lock(&mutx);
      sleep(1);
      printf("%s: REMOVE item from BUFFER %d\n", (char*)arg, index_counter);
      buffer[index_counter] = 0;
      index_counter--;
      pthread_mutex_unlock(&mutx);
    }
}

