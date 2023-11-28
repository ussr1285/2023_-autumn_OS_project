#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_READERS 5
#define NUM_WRITERS 2

int data = 0;
int reader_count = 0;
pthread_mutex_t mutex;
pthread_cond_t can_read, can_write;

// Reader function
void* reader(void* arg) {
    while(1) {
        pthread_mutex_lock(&mutex);
        while(reader_count == -1) {
            // Wait if a writer is writing
            pthread_cond_wait(&can_read, &mutex);
        }
        reader_count++; // Indicate reader is reading
        pthread_mutex_unlock(&mutex);

        // Read data
        printf("Reader %ld: Read data = %d\n", (long)arg, data);

        pthread_mutex_lock(&mutex);
        reader_count--;
        if(reader_count == 0) {
            // Signal writer if it's waiting
            pthread_cond_signal(&can_write);
        }
        pthread_mutex_unlock(&mutex);
        
        sleep(1); // Simulate work
    }
}

// Writer function
void* writer(void* arg) {
    while(1) {
        pthread_mutex_lock(&mutex);
        while(reader_count != 0) {
            // Wait if readers are reading
            pthread_cond_wait(&can_write, &mutex);
        }
        reader_count = -1; // Indicate writer is writing
        pthread_mutex_unlock(&mutex);

        // Write data
        data++;
        printf("Writer %ld: Wrote data = %d\n", (long)arg, data);

        pthread_mutex_lock(&mutex);
        reader_count = 0;
        // Signal readers and writers
        pthread_cond_broadcast(&can_read);
        pthread_cond_signal(&can_write);
        pthread_mutex_unlock(&mutex);

        sleep(2); // Simulate work
    }
}

int main() {
    pthread_t readers[NUM_READERS], writers[NUM_WRITERS];
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&can_read, NULL);
    pthread_cond_init(&can_write, NULL);

    // Create threads
    for(long i = 0; i < NUM_READERS; i++) {
        pthread_create(&readers[i], NULL, reader, (void*)i);
    }
    for(long i = 0; i < NUM_WRITERS; i++) {
        pthread_create(&writers[i], NULL, writer, (void*)i);
    }

    // Wait for threads to finish
    for(int i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }
    for(int i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writers[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&can_read);
    pthread_cond_destroy(&can_write);

    return 0;
}
