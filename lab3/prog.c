#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_SIZE 1024*8 //8Kb 
#define USER_THREAD_COUNT atoi(argv[1])

//globals
int MMS_id = 0;
pthread_mutex_t mutx;
sem_t mem_empty, mem_full;

//initialize functions
void* memory_malloc(int size){}
void memory_free(void *ptr, int size){}
void first_fit(){}
void best_fit(){}
void worst_fit(){}

typedef struct{
    int size;
    void* block_start;
} memBlock;

void main(int argc, char **argv){
    pthread_t MMS_thread, user_threads[USER_THREAD_COUNT];
    int state1, state2;
    if (argc != 2)
    {
        printf("Format must be: [file location] <int>\n");
        exit(0);
    }

    if(state1||state2!=0)
        puts("Error mutex & semaphore initialization!!!");
    pthread_create(&MMS_thread, NULL, first_fit, &MMS_id);
}

void* memory_malloc(int size){
    //this function calls the syustem function malloc to 
    //allocate memory
    pthread_mutex_lock(&mutx);

    pthread_mutex_unlock(&mutx);
    return NULL;
}

void memory_free(void *ptr, int size){
    //this function calls the system function free() to d
    //e-allocate memory
}
void first_fit(){

}
void best_fit(){

}
void worst_fit(){

}