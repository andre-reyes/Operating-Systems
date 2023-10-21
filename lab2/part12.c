#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// defining QUEUE size
#define size 10
int t7;
int r = 0;
int count;
// Thread callback function
void *producer(void *args);
void *consumer(void *args);

typedef struct
{

    int buffer[size];        // buffer size
    long h1, t1;             // head,tail
    int full, empty;         // full empty flags
    pthread_mutex_t *mutex;  // mutex locks
    pthread_cond_t *nF, *nE; // pointer for not full,not empty
} queue;

queue *queueInit(void);             // intialised queue
void qdelete(queue *q);             // delete item from queue
void qadd(queue *q, int in);        // add item to queue
void qdeletet1(queue *q, int *out); // delete from queue

int main(int argc, char **argv) // Main program
{
    // t7=argv[1]+1; //uncomment to use it with command line input
    printf("Enter the queue size: "); // user input
    scanf("%d", &t7);                 ////user input
    queue *f;                         // queue variable
    pthread_t produce, consume;
    f = queueInit(); // QUEUE initialise
    if (f == NULL)   // if thread is not initialised
    {
        fprintf(stderr, "main: Queue Initialisation failed.n");
        exit(1);
    }
    count = pthread_create(&produce, NULL, producer, f); // producer thread created
    if (count)                                           // if producer thread is not created
    {
        printf("n ERROR : pthread_create %d ", count);
        exit(-1);
    }
    count = pthread_create(&consume, NULL, consumer, f); // consumer thread created
    if (count)                                           // if consumer thread is not created
    {
        printf("n ERROR : pthread_create %d ", count);
        exit(-1);
    }
    count = pthread_join(produce, NULL); // join producer thread to main thread
    if (count)                           // if joins fail
    {
        printf("n ERROR : pthread_create %d ", count);
        exit(-1);
    }
    count = pthread_join(consume, NULL); // join consumer thread to main thread
    if (count)                           // if joins fail
    {
        printf("n ERROR : pthread_create %d ", count);
        exit(-1);
    }
    qdelete(f);
    return 0;
}
void *producer(void *q) // producer thread
{
    queue *f; // queue creation
    int i;
    f = (queue *)q; // queue variable
    for (i = 0; i < t7; i++)
    {
        r++;
        pthread_mutex_lock(f->mutex); // taking lock on mutex
        while (f->full)               // wait untill the space is not created
        {
            printf("producer: queue is full.n");
            pthread_cond_wait(f->nF, f->mutex);
        }
        qadd(f, i);                     // add element to queue
        pthread_mutex_unlock(f->mutex); // accuire the lock
        pthread_cond_signal(f->nE);     // signal the block consumer
        printf("(producer) Putting %d : %d items in the queue.n", i, r);
        usleep(100000);
    }

    return (NULL);
}
void *consumer(void *q) // consumer thread
{
    queue *f; // queue creation
    int i, d;

    f = (queue *)q; // queue variable
    for (i = 0; i < t7; i++)
    {
        r--;
        pthread_mutex_lock(f->mutex); // taking lock on mutex of buffer
        while (f->empty)              // wait until one item added to buffer
        {
            printf("consumer: queue is empty.n");
            pthread_cond_wait(f->nE, f->mutex); // signal the block producer to add item
        }
        qdeletet1(f, &d);               // remove item from queue
        pthread_mutex_unlock(f->mutex); // acquire lock
        pthread_cond_signal(f->nF);
        printf("(consumer) Getting %d : %d items in the queue.n", d, r);
        usleep(500000);
    }

    return (NULL);
}

#ifdef o
typedef struct
{
    int buffer[size];
    long h1, t1;
    int full, empty;
    pthread_mutex_t *mutex;
    pthread_cond_t *nF, *nE;
} queue;
#endif

queue *queueInit(void) // intialise the queue
{
    queue *q;                           // queue variable
    q = (queue *)malloc(sizeof(queue)); // creating a queue
    if (q == NULL)
        return (NULL);                                             // empty queue
    q->empty = 1;                                                  // flags
    q->full = 0;                                                   // flags
    q->h1 = 0;                                                     // flags
    q->t1 = 0;                                                     // flags
    q->mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)); // mutex initialised
    count = pthread_mutex_init(q->mutex, NULL);                    // if mutex is not initialised
    if (count)
    {
        printf("n ERROR : pthread_mutex_init %d ", count);
        exit(-1);
    }

    q->nF = (pthread_cond_t *)malloc(sizeof(pthread_cond_t)); // update non Full pointer
    pthread_cond_init(q->nF, NULL);
    q->nE = (pthread_cond_t *)malloc(sizeof(pthread_cond_t)); // update non empty pointer
    pthread_cond_init(q->nE, NULL);
    return (q);
}

void qdelete(queue *q)
{
    count = pthread_mutex_destroy(q->mutex); // mutex destroy
    if (count)                               // if mutex is not destroy
    {
        printf("n ERROR : pthread_mutex_destroy %d ", count);
        exit(-1);
    }
    free(q->mutex); // delete mutex
    pthread_cond_destroy(q->nF);
    free(q->nF);
    pthread_cond_destroy(q->nE);
    free(q->nE); // delete non empty pointer
    free(q);     // delete queue
}

void qadd(queue *q, int in)
{
    q->buffer[q->t1] = in; // buffer adding of item
    q->t1++;               // update read
    if (q->t1 == size)     // check front reached size of buffer read
        q->t1 = 0;         // empty
    if (q->t1 == q->h1)    // check rear reached size of buffer front
        q->full = 1;       // full
    q->empty = 0;
    return;
}

void qdeletet1(queue *q, int *out)
{

    *out = q->buffer[q->h1]; // buffer delete
    q->h1++;                 // update front
    if (q->h1 == size)       // front reach to rear
        q->h1 = 0;           // empty
    if (q->h1 == q->t1)      // read reach to front
        q->empty = 1;        // full
    q->full = 0;
    return;
}