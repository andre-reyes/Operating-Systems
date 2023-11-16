#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAX_SIZE 8000
#define MIN_SIZE (MAX_SIZE/10)

typedef struct memBlock{
    int isFree;
    int size; //size of the memory block
    int *address; 
    struct memBlock *next; //pointer to next block
}memBlock;
memBlock *block_list;

pthread_mutex_t lock;
sem_t empty, full;
int *used_memory;

void partition();
void displayBlocks();

void main(){
    //initialize
    srand(time(NULL));
    void *thread_result;
    pthread_t mms, user[USER_THREAD_COUNT];
    int user_id[USER_THREAD_COUNT];
    int index_counter = 1; //start from one to let mms be thread 0 always
    int MMS_id = 0;
    int state1 = pthread_mutex_init(&mutx, NULL);
    int state2 = sem_init(&sem, 0 ,1);
    used_memory = 0;

    //error handling
    if (argc != 2)
    {
        printf("Format must be: [file location] <int>\n");
        exit(0);
    }

    if(state1 || state2 != 0)
        puts("Error mutex & semaphore initialization!!!");

    //initialize memory block 
    block_list = (memBlock*)malloc(sizeof(memBlock));
    partition();
    displayBlocks();

    //create threads
    pthread_create(&mms, NULL, MMS_thread, &MMS_id);
    for(int i = 0; i < USER_THREAD_COUNT; i++){
        user_id[i] = index_counter;
        index_counter++;
        pthread_create(&user[i], NULL, user_thread, &user_id[i]);
    }

    // Waiting for thread to terminate
    pthread_join(mms, &thread_result);
    pthread_join(user, &thread_result);

    //destroy sem
    sem_destroy(&sem);
}
void *mms_thread(void* arg) {
    while (1) {
        sem_wait(&sem);
        if (used_memory >= MAX_SIZE) {
            printf("MMS running out of memory, forcing thread(s) to give up memory\n");
            pthread_mutex_lock(&lock);
            memBlock* current = block_list;
            while (current != NULL && used_memory >= MAX_SIZE) {
                used_memory -= current->size;
                printf("Freed memory of size %d bytes\n", current->size);
                current = block_list;
            }
            pthread_mutex_unlock(&lock);
        }
        sem_post(&sem);
    }
}
void *user_thread(void *arg){
    int size = rand()%(1024)+MIN_SIZE;
    int user_id = (int)arg;
    printf("Thread #%d requesting memory of size %d bytes\n", user_id, size);
    void *user_ptr = memory_malloc(size);
    if(user_ptr != NULL){
        //
        printf("I am thread #%d, going to sleep", user_id);
        sleep(rand() % 5 + 1);

        printf("I am thread #%d, waking-up", user_id);
        memory_free(usr_ptr, size);
        printf("I am thread #%d, freed %d bytes", user_id, size);
    }
    else{
        printf("Thread #%d failed to allocate \n", user_id);
    }
    //pthread_exit(NULL);

}
void* memory_malloc(int size) {
    first_fit(size);
    used_memory += size;
}
void memory_free(void* ptr, int size) {
    pthread_mutex_lock(&lock);
    memBlock *current = block_list;
    while(current != NULL && current->address < ptr){
        current = current->next;
    }
    current->size -= size;
    used_memory -= size;
    pthread_mutex_unlock(&lock);
}
void *first_fit(int size){
    pthread_mutex_lock(&lock);
    void *head = NULL;
    memBlock *current = block_list;
    while(current != NULL){
        if(size <= current->size && current->isFree){
            current->isFree = 0;
            pthread_mutex_unlock(&lock);
            head = current->address;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&lock);
    return head;
}
void partition(){
    memBlock *current = block_list;
    int size;
    int total_memory = MAX_SIZE;
    while (total_memory != 0){
        size = rand()%(1024)+MIN_SIZE;
        total_memory -= size;
        current->isFree = 1;
        current->size = size;
        // current->address = current;
        if(total_memory <= MIN_SIZE){
            current->size += total_memory;
            total_memory = 0;
            continue;
        }
        current->next = (memBlock*)malloc(sizeof(memBlock));
        current = current->next;
    }
}
void displayBlocks(){
    int block_id = 0;
    int total = 0;
    memBlock *current = block_list;

    printf("\n\t%10s | %14s | %8s | %-8s\n", "partitions", "address", "size (b)", "isFree");
    while(current != NULL){
        printf("\t%10d | %14p | %8d | %-8d\n", block_id, current, current->size, current->isFree);
        block_id++;
        total += current->size;
        current = current->next;
    }
    printf("Total Memory: %d\n", total);
}