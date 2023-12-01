#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "foodFactory.h"

#define STERILIZATION_KPA 121
#define STERILIZATION_CELSIUS_SCALE 103

int buffer[BUFFER_SIZE];
int sterilizedFoodCount = 0;
extern int foodCanCnt;
int bufferIn = -1;
int bufferOut = -1;

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
        // sleep(1); // 오류 내기 위한 sleep.
        int foodType;
        if (kPa >= STERILIZATION_KPA && celsiusScale >= STERILIZATION_CELSIUS_SCALE)
        {
            foodType = rand() % 100 + 1;
            bufferIn = (bufferIn + 1) % BUFFER_SIZE;
            buffer[bufferIn] = foodType;
            sterilizedFoodCount++;
            // printf("Produced: %d\n", foodType);
        }
        else
        {
            printf("Someting wrong at foodProducer.");
            exit(1);
        }
        sleep(1); // 작업 소요 시간.
    }
}

void* makeFoodCan(void* arg) {
    while(1) {
        while(sterilizedFoodCount == 0);
        int foodType;
        
        bufferOut = (bufferOut + 1) % BUFFER_SIZE;
        foodType = buffer[bufferOut];
        buffer[sterilizedFoodCount] = 0;
        printf("sterilizedFoodCount: %d\n", sterilizedFoodCount);
        sterilizedFoodCount--;
        // printf(" Consumed: %d\n", foodType);
        foodCanCnt++;
        printf("foodCan: %d\n", foodCanCnt);
        sleep(1); // 작업 소요시간
    }
}
