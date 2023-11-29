#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "foodFactory.h"

#define MAXLINE 4096
#define MSG_SIZE 10000
#define FIFO1 "/tmp/fifo.1"
#define FIFO2 "/tmp/fifo.2"

void settingInputs(char *msg, int *amountFoodProducer, int *packingMachine);
void controlInput(char *msg, char *controlVar);

int main(void) {
    char msg[MSG_SIZE];
    int readfd;
    int writefd;
    int nread;
    int amountFoodProducer;
    int packingMachine;
    char readBuffer[MAXLINE];
    int firstInput = 1;
    char controlVar[2] = " ";

	if ((readfd = open(FIFO2, O_RDWR)) < 0) {
		write(1, "fail to open named pipe\n", 24);
		exit(1);
	}
    if ((writefd = open(FIFO1, O_WRONLY)) < 0) {
        write(1, "fail to open named pipe\n", 24);
        exit(1);
    }

    while(1) {
        if (firstInput)
        {
            settingInputs(msg, &amountFoodProducer, &packingMachine);
            firstInput = 0;
        }
        else
            controlInput(msg, controlVar);
        if ((nread = write(writefd, msg, sizeof(msg))) < 0 ) { 
            write(1, "fail to call write()\n", 21);
            exit(1);
        }
        read(readfd, readBuffer, sizeof(readBuffer));
        printf("%s\n", readBuffer);
    }
    return 0;
}

void settingInputs(char *msg, int *amountFoodProducer, int *packingMachine)
{
    printf("가동할 통조림 음식 멸균기의 개수를 입력해주세요: ");
    scanf("%d", amountFoodProducer);
    strcpy(msg, ft_itoa(*amountFoodProducer));
    strcat(msg, "\n");
    printf("가동할 통조림 포장기의 개수를 입력해주세요: ");
    scanf("%d", packingMachine);
    strcat(msg, ft_itoa(*packingMachine));
    // strcat(msg, "\n");
	return ;
}

void controlInput(char *msg, char *controlVar)
{
    printf("\na. 현재 가동 중인 기계 확인.\nb. 현재까지 생산된 통조림 수 확인.\nc. 기계 동작을 중지.\n명령: ");
    scanf(" %c", &controlVar[0]);
    strcpy(msg, controlVar);
}
