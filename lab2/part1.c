#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <unistd.h>

#define BUYER_THREAD_COUNT  atoi(argv[1])// buyer thread count command-line argument
#define BUFFER_SIZE 10      // maximum size of the queue

int queue[BUFFER_SIZE];
int buffer_counter = 0;
pthread_cond_t queue_available = PTHREAD_COND_INITIALIZER;
pthread_cond_t queue_full = PTHREAD_COND_INITIALIZER;
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;

void *provider_insert(void *arg);
void *buyer_remove(void *arg); 

char provider1_id[] = "Provider A";
char provider2_id[] = " Provider B";

sem_t bin_sem;	// semaphore

int main(int argc, char **argv)
{
    pthread_t providers[2], buyers[BUYER_THREAD_COUNT];
    void *thread_result;


    int buyer_id[BUYER_THREAD_COUNT];
    int state1, state2;
    
    // make sure we have right number or format of args in command line
    if (argc != 2)
    {
        printf("Format must be:  [file location] <int>\n");
        exit(0);
    }

    //semaphore initialization, first value = 0
    state1 = pthread_mutex_init(&queue_mutex, NULL);
    state2 = sem_init(&bin_sem, 0 ,0);
    if(state1||state2!=0)
        puts("Error mutex & semaphore initialization!!!");

    // create provider threads
    pthread_create(&providers[0], NULL, provider_insert, &provider1_id[0]);
    pthread_create(&providers[1], NULL, provider_insert, &provider2_id[1]);

    // create buyer threads
    for (i = 0; i < BUYER_THREAD_COUNT; i++){
        buyer_id[i] = i;
        pthread_create(&buyers[i], NULL, buyer_remove, &buyer_id[i]);
    }

    // waiting for provider and buyer threads to terminate
    pthread_join(providers[0], &thread_result);
    pthread_join(providers[1], &thread_result);
    
    for (i = 0; i < BUYER_THREAD_COUNT; i++){
        pthread_join(buyers[i], &thread_result);
    }
    
    printf("Terminate!!!\n");
    sem_destroy(&bin_sem);	// destroy semaphore
    pthread_mutex_destroy(&queue_mutex);	// destroy mutex
    return 0;
}

void *provider_insert(void *arg)
{
    int item;
    for(int i=0;i<BUFFER_SIZE;i++)
    {
        // produce an item
        item = rand() % 10 + 1; // produce item 1-100
        pthread_mutex_lock(&queue_mutex);

        // wait until the queue is not full
        while (buffer_counter  == BUFFER_SIZE)
        {
            pthread_cond_wait(&queue_available, &queue_mutex);
        }

        // insert the item into the queue
        queue[buffer_counter++] = item;
        printf("%s produced item %d\n", (char*)arg, item);
        sem_post(&bin_sem);	// semaphore to increase

        pthread_cond_signal(&queue_full);
        pthread_mutex_unlock(&queue_mutex);

        // sleep
        sleep(2);
    }
    return NULL;
}

void *buyer_remove(void *arg)
{
    int item;
    for(int i=0;i<BUFFER_SIZE;i++)
    {
        sem_wait(&bin_sem);	//decrease index_counter
        pthread_mutex_lock(&queue_mutex);
        // wait until the queue is not empty
        while (buffer_counter == 0)
        {
            pthread_cond_wait(&queue_full, &queue_mutex);
        }

        // buy an item from the queue
        item = queue[--buffer_counter];
        printf("Buyer %d bought item %d\n", *((int *)arg), item);

        pthread_cond_signal(&queue_available);
        pthread_mutex_unlock(&queue_mutex);

        // sleep
        sleep(1);
    }
    return NULL;
}