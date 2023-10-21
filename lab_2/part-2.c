#include <stdio.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>

#define buffersize 10
#define NUM_PROVIDER 4
#define NUM_BUYER 260

pthread_mutex_t mutex;
pthread_t tidP[NUM_PROVIDER], tidB[NUM_BUYER];
sem_t full, empty;

int counter;
int buffer[buffersize];

void initialize(){
    pthread_mutex_init(&mutex, NULL);

    sem_init(&full, 1, 0);

    sem_init(&empty, 1, buffersize);

    counter = 0;
}

void write(int item)

{
    buffer[counter++] = item;
}

int read()

{

    return (buffer[--counter]);
}

void *provider(void *param)

{

    int waittime, item, i;

    srand(time(NULL));

    item = rand() % 5000;

    waittime = rand() % 5;

    sem_wait(&empty);

    pthread_mutex_lock(&mutex);

    printf("\nProvider has added item: %d\n", item);

    write(item);

    pthread_mutex_unlock(&mutex);

    sem_post(&full);
}

void *buyer(void *param)

{

    int waittime, item;

    waittime = rand() % 5;

    sem_wait(&full);

    pthread_mutex_lock(&mutex);

    item = read();

    printf("\nBuyer has bought item: %d\n", item);

    pthread_mutex_unlock(&mutex);

    sem_post(&empty);
}

int main()

{

    int i;

    initialize();

    for (i = 0; i < NUM_PROVIDER; i++)

        pthread_create(&tidP[i], NULL, provider, NULL);

    for (i = 0; i < NUM_BUYER; i++)

        pthread_create(&tidB[i], NULL, buyer, NULL);

    for (i = 0; i < NUM_PROVIDER; i++)

        pthread_join(tidP[i], NULL);

    for (i = 0; i < NUM_BUYER; i++)

        pthread_join(tidB[i], NULL);

    return 0;
}