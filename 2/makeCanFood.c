#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "foodFactory.h"

#define STERILIZATION_KPA 121
#define STERILIZATION_CELSIUS_SCALE 103

int buffer[BUFFER_SIZE];
int sterilizedFoodCount = 0;
extern int foodCanCnt;
int bufferIn = 0;
int bufferOut = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // 뮤텍스 초기화
pthread_cond_t buffer_not_full = PTHREAD_COND_INITIALIZER; // 조건 변수 초기화
pthread_cond_t buffer_not_empty = PTHREAD_COND_INITIALIZER; // 조건 변수 초기화

void* foodProducer(void* arg) {
    int kPa = 0; // 기압
    int celsiusScale = 0; // 섭씨 온도.

    // 멸균을 위한 Autoclave 세팅 준비.
    while (celsiusScale <= STERILIZATION_CELSIUS_SCALE)
        celsiusScale++;
    while (kPa < STERILIZATION_KPA)
        kPa++;
    while(1) {
        while(sterilizedFoodCount == BUFFER_SIZE)
            pthread_cond_wait(&buffer_not_full, &mutex); // 버퍼가 가득 찼을 때 대기
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

        pthread_cond_signal(&buffer_not_empty); // 버퍼가 비어있지 않다는 신호
        pthread_mutex_unlock(&mutex); // 뮤텍스 해제

        sleep(1); // 작업 소요 시간.
    }
}

void* makeFoodCan(void* arg) {
    while(1) {
        pthread_mutex_lock(&mutex); // 뮤텍스 잠금
        
        while(sterilizedFoodCount == 0)
            pthread_cond_wait(&buffer_not_empty, &mutex); // 버퍼가 비었을 때 대기
        int foodType;

        bufferOut = (bufferOut + 1) % BUFFER_SIZE;
        foodType = buffer[bufferOut];
        sterilizedFoodCount--;
        buffer[bufferOut] = 0;
        printf("sterilizedFoodCount: %d\n", sterilizedFoodCount + 1);
        
        // printf(" Consumed: %d\n", foodType);

        foodCanCnt++;
        printf("foodCan: %d\n", foodCanCnt);

        pthread_cond_signal(&buffer_not_full); // 버퍼가 가득 차지 않았다는 신호
        pthread_mutex_unlock(&mutex); // 뮤텍스 해제
        sleep(1); // 작업 소요시간
    }
}
