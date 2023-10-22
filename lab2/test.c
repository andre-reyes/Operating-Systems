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

#define BUFFER_SIZE 10
#define BUYER_THREAD_COUNT  atoi(argv[1])// buyer thread count command-line argument

typedef int buffer_item;

buffer_item buffer[BUFFER_SIZE];

// global variable, all threads can acess
int index_counter = 0;
pthread_cond_t queue_available = PTHREAD_COND_INITIALIZER;
pthread_cond_t queue_not_empty = PTHREAD_COND_INITIALIZER;

void *provider_insert(void *arg);	// function for sending
void *buyer_remove(void *arg);	// function for receiving

sem_t bin_sem;	// semaphore
pthread_mutex_t mutx;	// mutex

char provider1[]="provider A";
char provider2[]="provider B";


int main(int argc, char **argv)
{
    pthread_t t1, t2, buyers[BUYER_THREAD_COUNT];
    void *thread_result;
    int state1, state2;
    int buyer_id[BUYER_THREAD_COUNT];
    state1 = pthread_mutex_init(&mutx, NULL);
    state2 = sem_init(&bin_sem, 0 ,0);
    //mutex initialization
    //semaphore initialization, first value = 0
    // make sure we have right number or format of args in command line
    if (argc != 2)
    {
        printf("Format must be:  [file location] <int>\n");
        exit(0);
    }
    if(state1||state2!=0)
        puts("Error mutex & semaphore initialization!!!");

    // Create provider1, provider2, thread3
    pthread_create(&t1, NULL, provider_insert, &provider1);
    pthread_create(&t2, NULL, provider_insert, &provider2);
    for(int i = 0;i < BUYER_THREAD_COUNT; i++){
        buyer_id[i] = i;
        pthread_create(&buyers[i], NULL, buyer_remove, &buyer_id[i]);
    }
    // Waiting thread to terminate
    pthread_join(t1, &thread_result);
    pthread_join(t2, &thread_result);
    for(int i = 0;i < BUYER_THREAD_COUNT; i++){
        pthread_join(buyers[i], &thread_result);
    }

    printf("Terminate => %s, %s!!!\n", &provider1, &provider2);
    printf("Final Index: %d\n", index_counter);

    sem_destroy(&bin_sem);	// destroy semaphore
    pthread_mutex_destroy(&mutx);	// destroy mutex
    
    return 0;
}

// Thread increases item
void *provider_insert(void *arg)
{
  printf("Creating Thread: %s\n", (char*)arg);
  
  for(int i=0;i<BUFFER_SIZE;i++)
  {
    pthread_mutex_lock(&mutx);
    // wait until the queue is not full
    while (index_counter  == BUFFER_SIZE)
    {
        pthread_cond_wait(&queue_available, &mutx);
    }
    if(index_counter<BUFFER_SIZE)
    {
      buffer[index_counter] = index_counter;
      index_counter++;
      
      printf("%s produced item %d\n", (char*)arg, index_counter);

      sem_post(&bin_sem);	// semaphore to increase
      pthread_cond_signal(&queue_not_empty);
    }
    else
    {
      sleep(2);
    }
    pthread_mutex_unlock(&mutx);
  }
}

// Thread decreases item
void *buyer_remove(void *arg)
{
  while (index_counter == 0)
      {
          pthread_cond_wait(&queue_not_empty, &queue_mutex);
      }
  for(int i=0;i<BUFFER_SIZE;i++)
  {
    sem_wait(&bin_sem);	//decrease index_counter
    pthread_mutex_lock(&mutx);
    sleep(1);
    printf("Buyer %d bought item %d\n", *((int *)arg), index_counter);
    buffer[index_counter] = 0;
    index_counter--;
    pthread_mutex_unlock(&mutx);
    }
}

