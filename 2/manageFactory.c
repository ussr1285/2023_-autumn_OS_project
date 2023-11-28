#include <pthread.h>

void* foodProducer(void* arg);
void* makeFoodCan(void* arg);

int main(int argc, char *argv[]) {
    pthread_t prod_thread, cons_thread;

    // Create producer threads // Create consumer threads
    pthread_create(&prod_thread, NULL, foodProducer, NULL);
    pthread_create(&cons_thread, NULL, makeFoodCan, NULL);

    // Wait for the threads to finish (they won't in this example)
    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);
    return 0;
}
