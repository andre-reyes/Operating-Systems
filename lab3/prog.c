#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_SIZE 1024*8 //8KB 
#define USER_THREAD_COUNT atoi(argv[1])

//globals
int MMS_id = 0;
pthread_mutex_t mutx;
sem_t sem;
pthread_cond_t mem_empty, mem_full;


//declare functions

void user_thread();

void MMS_thread();
//memory_malloc calls the system function malloc() to allocate memory
void *memory_malloc(int size);
//memory_free calls the system function free() to de-allocate memory
void memory_free(void *ptr, int size);
void first_fit();
void best_fit();
void worst_fit();

typedef struct{
    int size;
    void *start;
} memBlock;

void main(int argc, char **argv){
    void *thread_result;
    pthread_t mms, user[USER_THREAD_COUNT];
    int user_id[USER_THREAD_COUNT];
    char mms_id = "MMS Thread";
    int state1 = pthread_mutex_init(&mutx, NULL);
    int state2 = sem_init(&sem, 0 ,0);

    if (argc != 2)
    {
        printf("Format must be: [file location] <int>\n");
        exit(0);
    }

    if(state1 || state2 != 0)
        puts("Error mutex & semaphore initialization!!!");

    //create threads
    pthread_create(&mms, NULL, MMS_thread, &MMS_id);
    for(int i = 0; i < USER_THREAD_COUNT; i++){
        user_id[i] = i;
        pthread_create(&user[i], NULL, user_thread, &user_id[i]);
    }

    // Waiting for thread to terminate
    pthread_join(mms, &thread_result);
    pthread_join(user, &thread_result);

    //destroy sem
    sem_destroy(&sem);
    return 0;
}

void user_thread(){

}
void MMS_thread(){
    while(1){
        printf("insert desired");
    }
}
void *memory_malloc(int size){
    pthread_mutex_lock(&mutx);
    memBlock *block_start = malloc(MAX_SIZE);
    
    pthread_mutex_unlock(&mutx);
    return NULL;
}

void memory_free(void *ptr, int size){
    pthread_mutex_lock(&mutx);

    pthread_mutex_unlock(&mutx);
}
void first_fit(){

}
void best_fit(){

}
void worst_fit(){

}