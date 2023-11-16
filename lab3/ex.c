#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_SIZE 1024*1024*10 // 10 MB
#define NUM_THREADS 5

typedef struct {
    int size;
    void* start;
} MemoryBlock;

MemoryBlock* freeList;
pthread_mutex_t lock;
sem_t empty, full;
int totalMemory;

void* memory_malloc(int size) {
    pthread_mutex_lock(&lock);
    MemoryBlock* curr = freeList;
    MemoryBlock* prev = NULL;
    while (curr != NULL) {
        if (curr->size >= size) {
            void* start = curr->start;
            if (curr->size == size) {
                if (prev == NULL) {
                    freeList = curr->next;
                } else {
                    prev->next = curr->next;
                }
            } else {
                curr->start += size;
                curr->size -= size;
            }
            totalMemory -= size;
            pthread_mutex_unlock(&lock);
            return start;
        }
        prev = curr;
        curr = curr->next;
    }
    pthread_mutex_unlock(&lock);
    return NULL;
}

void memory_free(void* ptr, int size) {
    pthread_mutex_lock(&lock);
    MemoryBlock* curr = freeList;
    MemoryBlock* prev = NULL;
    while (curr != NULL && curr->start < ptr) {
        prev = curr;
        curr = curr->next;
    }
    if (prev == NULL) {
        freeList = (MemoryBlock*) ptr;
    } else {
        prev->next = (MemoryBlock*) ptr;
    }
    ((MemoryBlock*) ptr)->next = curr;
    ((MemoryBlock*) ptr)->size = size;
    totalMemory += size;
    pthread_mutex_unlock(&lock);
}

void* user_thread(void* arg) {
    int id = (int) arg;
    int size = rand() % MAX_SIZE + 1;
    printf("Thread #%d requesting memory of size %d bytes\n", id, size);
    void* ptr = memory_malloc(size);
    if (ptr == NULL) {
        printf("Thread #%d failed to allocate memory\n", id);
    } else {
        printf("Thread #%d allocated memory at %p\n", id, ptr);
        int sleep_time = rand() % 10 + 1;
        printf("Thread #%d sleeping for %d seconds\n", id, sleep_time);
        sleep(sleep_time);
        printf("Thread #%d waking up\n", id);
        memory_free(ptr, size);
        printf("Thread #%d freed memory\n", id);
    }
    pthread_exit(NULL);
}

void* mms_thread(void* arg) {
    while (1) {
        sem_wait(&full);
        if (totalMemory >= MAX_SIZE) {
            printf("MMS running out of memory, forcing thread(s) to give up memory\n");
            pthread_mutex_lock(&lock);
            MemoryBlock* curr = freeList;
            while (curr != NULL && totalMemory >= MAX_SIZE) {
                freeList = curr->next;
                totalMemory -= curr->size;
                printf("Freed memory of size %d bytes\n", curr->size);
                curr = freeList;
            }
            pthread_mutex_unlock(&lock);
        }
        sem_post(&empty);
    }
}

int main() {
    freeList = (MemoryBlock*) malloc(sizeof(MemoryBlock));
    return 0;
}