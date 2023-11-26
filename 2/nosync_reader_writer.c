#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_READERS 5
#define NUM_WRITERS 2

int data = 0;

// Reader function
void* reader(void* arg) {
    while(1) {
        int read_data = data; // Read data
        printf("Reader %ld: Read data = %d\n", (long)arg, read_data);
        sleep(1); // Simulate work
    }
}

// Writer function
void* writer(void* arg) {
    while(1) {
        data++; // Write data
        printf("Writer %ld: Wrote data = %d\n", (long)arg, data);
        sleep(2); // Simulate work
    }
}

int main() {
    pthread_t readers[NUM_READERS], writers[NUM_WRITERS];

    // Create reader threads
    for(long i = 0; i < NUM_READERS; i++) {
        pthread_create(&readers[i], NULL, reader, (void*)i);
    }

    // Create writer threads
    for(long i = 0; i < NUM_WRITERS; i++) {
        pthread_create(&writers[i], NULL, writer, (void*)i);
    }

    // Wait for threads to finish (they won't in this example)
    for(int i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }
    for(int i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writers[i], NULL);
    }

    return 0;
}
