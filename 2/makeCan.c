#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "foodFactory.h"

#define STERILIZATION_KPA 121
#define STERILIZATION_CELSIUS_SCALE 103

int buffer[BUFFER_SIZE];
int count = 0;

// Producer function
void* foodProducer(void* arg) {
    int kPa = 0; // 기압
    int celsiusScale = 0; // 섭씨 온도.

    // 멸균을 위한 Autoclave 세팅 준비.
    while (celsiusScale <= STERILIZATION_CELSIUS_SCALE)
        celsiusScale++;
    while (kPa < STERILIZATION_KPA)
        kPa++;
    while(1) {
        while(count == BUFFER_SIZE);
        int item;
        // Produce an item
        if (kPa >= STERILIZATION_KPA && celsiusScale >= STERILIZATION_CELSIUS_SCALE)
        {
            item = rand() % 100; // food_quality?!
            buffer[count] = item;
            count++;
            printf("Produced: %d\n", item);
        }
        else
        {
            printf("Someting wrong at foodProducer.");
            exit(1);
        }
        sleep(1);
    }
}

// Consumer function
void* makeCan(void* arg) {
    while(1) {
        while(count == 0); // Wait if buffer is empty

        // Consume an item
        count--;
        int item = buffer[count];
        
        printf("Consumed: %d\n", item);
        sleep(1); // Simulate work
    }
}