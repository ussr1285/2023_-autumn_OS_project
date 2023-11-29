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

    // Create producer threads // Create consumer threads
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

    // Wait for the threads to finish (they won't in this example)
    i = -1;
    while(++i < prod_size)
    {
        pthread_join(prod_thread[i], NULL);
    }
    i = -1;
    while(++i < cons_size)
    {
        pthread_join(cons_thread[i], NULL);
    }
    return 0;
}

int main(void) {
	char msg[MSG_SIZE];
	int fd;
	int readfd;
	int writefd;
	int nread;
    char foodProducerBuffer[MAXLINE];
    char actionBuffer;
    char byteBuffer[10];
	char dataBuffer[MAXLINE];
	char temp_msg[MSG_SIZE];
	char lenBuffer[MAXLINE];
	size_t n;
	pid_t childpid;

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
		if ((nread = read(readfd, msg, sizeof(msg))) < 0 ) {
			write(1, "fail to call read()\n", 20);
			exit(1);
		}
		if(msg[0] != '\0')
		{
            strcpy(temp_msg, msg);
            strcpy(foodProducerBuffer, strtok(temp_msg, "\n"));
            actionBuffer = strtok(0, "\n")[0];
            if(actionBuffer == 'r')
                strcpy(byteBuffer, strtok(0, "\n"));
            else
            {
                write(writefd, "Please enter correct order.\n", 28);
                continue;
            }
            printf("filename: %s action: %c byte: %s data: %s\n", foodProducerBuffer, actionBuffer, byteBuffer, dataBuffer);
            
            if(actionBuffer == 'r')
            {

            }
            else if(actionBuffer == 'w')
            {

            }
            else
            {
				write(writefd, "Please enter correct order.\n", 28);
                continue;
            }
		}
	}
	return 0;
}
