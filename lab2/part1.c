#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define PROVIDER_THREAD_COUNT 2    // provider thread count
#define BUYER_THREAD_COUNT atoi(argv[1]) // buyer thread count command-line argument
#define BUFFER 20      // maximum size of the queue

int queue[BUFFER];
int buffer = 0;
pthread_cond_t queue_empty = pthread_cond_init;
pthread_cond_t queue_full = PTHREAD_COND_INITIALIZER;
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;

void *provider_insert(void *arg);
void *buyer_remove(void *arg); 

int main(int argc, char **argv)
{
    int i;

    int provider_ids[PROVIDER_THREAD_COUNT];
    int buyer_ids[BUYER_THREAD_COUNT];
    pthread_t providers[PROVIDER_THREAD_COUNT];
    pthread_t buyers[BUYER_THREAD_COUNT];

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <number of buyers>\n", argv[0]);
        exit(0);
    }

    // create provider threads
    for (i = 0; i < PROVIDER_THREAD_COUNT; i++)
    {
        provider_ids[i] = i;
        pthread_create(&providers[i], NULL, provider_insert, (void *)&provider_ids[i]);
    }

    // create buyer threads
    for (i = 0; i < BUYER_THREAD_COUNT; i++)
    {
        buyer_ids[i] = i;
        pthread_create(&buyers[i], NULL,
                       buyer_remove, (void *)&buyer_ids[i]);
    }

    // wait for the provider and buyer threads to finish
    for (i = 0; i < PROVIDER_THREAD_COUNT; i++)
    {
        pthread_join(providers[i], NULL);
    }
    for (i = 0; i < BUYER_THREAD_COUNT; i++)
    {
        pthread_join(buyers[i], NULL);
    }

    return 0;
}

void *provider_insert(void *arg)
{
    int item;
    while (1)
    {
        // produce an item
        item = rand() % atoi(argv[1]);
        pthread_mutex_lock(&queue_mutex);

        // wait until the queue is not full
        while (buffer == BUFFER)
        {
            pthread_cond_wait(&queue_empty, &queue_mutex);
        }

        // insert the item into the queue
        queue[buffer++] = item;
        printf("Provider %d produced item %d\n", *((int *)arg), item);

        pthread_cond_signal(&queue_full);
        pthread_mutex_unlock(&queue_mutex);

        // sleep
        sleep(1);
    }

    return NULL;
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

        pthread_cond_signal(&queue_empty);
        pthread_mutex_unlock(&queue_mutex);

        // sleep
        sleep(1);
    }

    return NULL;
}