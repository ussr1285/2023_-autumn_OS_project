#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 10

int buffer[BUFFER_SIZE];
int count = 0;

// Producer function
void* producer(void* arg) {
    while(1) {
        while(count == BUFFER_SIZE); // Wait if buffer is full

        // Produce an item
        int item = rand() % 100;
        buffer[count] = item;
        count++;

        printf("Produced: %d\n", item);
        sleep(1); // Simulate work
    }
}

// Consumer function
void* consumer(void* arg) {
    while(1) {
        while(count == 0); // Wait if buffer is empty

        // Consume an item
        count--;
        int item = buffer[count];
        
        printf("Consumed: %d\n", item);
        sleep(1); // Simulate work
    }
}

int main() {
    pthread_t prod_thread, cons_thread;

    // Create producer and consumer threads
    pthread_create(&prod_thread, NULL, producer, NULL);
    pthread_create(&cons_thread, NULL, consumer, NULL);

    // Wait for the threads to finish (they won't in this example)
    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    return 0;
}
