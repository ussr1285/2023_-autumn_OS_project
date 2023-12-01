#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "foodFactory.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#define MAXLINE 4096
#define MSG_SIZE 10000
#define FIFO1 "/tmp/fifo.1"
#define FIFO2 "/tmp/fifo.2"
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int foodCanCnt = 0;
int foodCanBoxCnt = 0;

extern void* foodProducer(void* arg);
extern void* makeFoodCan(void* arg);

int runServer(int prod_size, int cons_size) {
    pthread_t *prod_thread;
    pthread_t *cons_thread;
    int i;

    prod_thread = (pthread_t *) malloc(sizeof(pthread_t) * prod_size);
    cons_thread = (pthread_t *) malloc(sizeof(pthread_t) * cons_size);
    
    i = -1;
    while(++i < prod_size)
    {
        pthread_create(&(prod_thread[i]), NULL, foodProducer, NULL);
    }
    i = -1;
    while(++i < cons_size)
    {
        pthread_create(&(cons_thread[i]), NULL, makeFoodCan, NULL);
    }

    i = -1;
    return 0;
}

int main(void) {
	char msg[MSG_SIZE];
    char sendMsg[MSG_SIZE];
	int fd;
	int readfd;
	int writefd;
	int nread;
    int amountFoodProducer;
    int packingMachine;
	char temp_msg[MSG_SIZE];
	char lenBuffer[MAXLINE];
	size_t n;
	pid_t childpid;
    int firstRun = 1;
    char controlVar;

	if (access(FIFO1,F_OK) == 0) {
		unlink(FIFO1);
	}
	if (access(FIFO2,F_OK) == 0) {
		unlink(FIFO2);
	}
	if ((mkfifo(FIFO1,FILE_MODE)) < 0) {
		write(1, "fail to make named pipe\n", 24);
		exit(1);
	}
	if ((mkfifo(FIFO2,FILE_MODE)) < 0) {
		write(1, "fail to make named pipe\n", 24);
		exit(1);
	}
	if ((readfd = open(FIFO1, O_RDWR)) < 0) {
		write(1, "fail to open named pipe\n", 24);
		exit(1);
	}
    if ((writefd = open(FIFO2, O_WRONLY)) < 0) {
        write(1, "fail to open named pipe\n", 24);
        exit(1);
    }
	while (1) {
        if (firstRun == 1)
        {
            if ((nread = read(readfd, msg, sizeof(msg))) < 0 ) {
                write(1, "fail to call read()\n", 20);
                exit(1);
            }
            if(msg[0] != '\0') {
                strcpy(temp_msg, msg);
                amountFoodProducer = atoi(strtok(temp_msg, "\n"));
                packingMachine = atoi(strtok(0, "\n"));
                runServer(amountFoodProducer, packingMachine);
                sprintf(sendMsg, "Foodcan factory running now.");
                write(writefd, sendMsg, strlen(sendMsg));
            }
            firstRun = 0;
            msg[0] = '\0';
        }
        else
        {
            if ((nread = read(readfd, msg, sizeof(msg))) < 0 ) {
                write(1, "fail to call read()\n", 20);
                exit(1);
            }
            if(msg[0] != '\0') {
                controlVar = msg[0];
                if(controlVar == 'a')
                {
                    sprintf(sendMsg, "통조림 음식 멸균기: %d, 통조림 포장기계: %d\n",amountFoodProducer, packingMachine);
                }
                else if(controlVar == 'b'){
                    sprintf(sendMsg, "현재까지 생산된 통조림: %d\n", foodCanCnt);
                }
                else if(controlVar == 'c')
                {
                    sprintf(sendMsg, "공장 가동을 종료합니다.\n");
                    printf("%s\n", sendMsg);
                    write(writefd, sendMsg, strlen(sendMsg));
                    exit(1);
                }
                else if(msg[0] >= 0 && msg[0] <= 127)
                {
                    printf("msg: %s\n", msg);
                    sprintf(sendMsg, "올바른 명령을 입력해주세요.\n");
                }
                msg[0] = '\0';
                write(writefd, sendMsg, strlen(sendMsg));
            }
        }
	}
	return 0;
}
