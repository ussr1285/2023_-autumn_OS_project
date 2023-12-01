#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "foodFactory.h"

#define STERILIZATION_KPA 121
#define STERILIZATION_CELSIUS_SCALE 103
#define FOOD_CAN_PER_BOX 8

int buffer[BUFFER_SIZE];
int sterilizedFoodCount = 0;
extern int foodCanCnt;
extern int foodCanBoxCnt;

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
        while(sterilizedFoodCount == BUFFER_SIZE);
        int item;
        // Produce an item
        if (kPa >= STERILIZATION_KPA && celsiusScale >= STERILIZATION_CELSIUS_SCALE)
        {
            item = rand() % 100; // food_quality?!
            buffer[sterilizedFoodCount] = item;
            sterilizedFoodCount++;
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
void* makeFoodCan(void* arg) {
    while(1) {
        while(sterilizedFoodCount == 0); // Wait if buffer is empty

        // Consume an item
        sterilizedFoodCount--;
        int item = buffer[sterilizedFoodCount];
        
        printf("Consumed: %d\n", item);
        foodCanCnt++;
        // if (foodCanCnt >= FOOD_CAN_PER_BOX)
        // {
        //     foodCanCnt %= FOOD_CAN_PER_BOX;
        //     foodCanBoxCnt++;
        // }
        // printf("foodCan: %d, foodCanBox: %d\n", foodCanCnt, foodCanBoxCnt);
        printf("foodCan: %d\n", foodCanCnt);
        sleep(1); // Simulate work
    }
}
