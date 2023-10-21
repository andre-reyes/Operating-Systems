#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUYER_THREAD_COUNT  atoi(argv[1])// buyer thread count command-line argument
#define BUFFER_SIZE 20      // maximum size of the queue

int queue[BUFFER_SIZE];
int buffer = 0;
pthread_cond_t queue_available = PTHREAD_COND_INITIALIZER;
pthread_cond_t queue_full = PTHREAD_COND_INITIALIZER;
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t()

void *provider_insert(void *arg);
void *buyer_remove(void *arg); 

char provider1_id[] = "Provider A";
char provider2_id[] = "Provider B";


int main(int argc, char **argv)
{
    pthread_t providers[2], buyers[BUYER_THREAD_COUNT];


    int buyer_id[BUYER_THREAD_COUNT];
    int i;
    
    // make sure we have right number or format of args in command line
    if (argc != 2)
    {
        printf("Format must be:  [file location] <int>\n");
        exit(0);
    }

    // create provider threads
    
    pthread_create(&providers[0], NULL, provider_insert, &provider1_id[0]);
    pthread_create(&providers[1], NULL, provider_insert, &provider2_id[1]);
    

    // create buyer threads
    for (i = 0; i < BUYER_THREAD_COUNT; i++)
    {
        buyer_id[i] = i;
        pthread_create(&buyers[i], NULL, buyer_remove, &buyer_id[i]);
    }

    // wait for the provider and buyer threads to finish
    for (i = 0; i < 2; i++)
    {
        pthread_join(providers[i], NULL);
    }
    for (i = 0; i < BUYER_THREAD_COUNT; i++)
    {
        pthread_join(buyers[i], NULL);
    }
    pthread
    return 0;
}

void *provider_insert(void *arg)
{
    int item;
    while (1)
    {
        // produce an item
        item = rand() % 100 + 1; // produce item 1-100
        pthread_mutex_lock(&queue_mutex);

        // wait until the queue is not full
        while (buffer == BUFFER_SIZE)
        {
            pthread_cond_wait(&queue_available, &queue_mutex);
        }

        // insert the item into the queue
        queue[buffer++] = item;
        printf("%s produced item %d\n", (char*)arg, item);

        pthread_cond_signal(&queue_full);
        pthread_mutex_unlock(&queue_mutex);

        // sleep
        sleep(1);
    }
}

void *buyer_remove(void *arg)
{
    int item;
    while (1)
    {
        pthread_mutex_lock(&queue_mutex);

        // wait until the queue is not empty
        while (buffer == 0)
        {
            pthread_cond_wait(&queue_full, &queue_mutex);
        }

        // buy an item from the queue
        item = queue[--buffer];
        printf("Buyer %d bought item %d\n", *((int *)arg), item);

        pthread_cond_signal(&queue_available);
        pthread_mutex_unlock(&queue_mutex);

        // sleep
        sleep(1);
    }
}