#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAXLINE 4096
#define MSG_SIZE 10000
#define FIFO1 "/tmp/fifo.1"
#define FIFO2 "/tmp/fifo.2"
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

// void fileReadResponse(int readfd, int writefd);
// void fileWriteResponse();

int main(void) {
	char msg[MSG_SIZE];
	int fd;
	int readfd;
	int writefd;
	int nread;
    char fileNameBuffer[MAXLINE];
    char actionBuffer;
    char byteBuffer[10];
	char dataBuffer[MAXLINE];
	char temp_msg[MSG_SIZE];
	char data[MAXLINE];
	size_t n;
	// int childpid;
	// int childStatus;
	// int ret;

	if (access(FIFO1,F_OK) == 0) {
		unlink(FIFO1);
	}
	if (access(FIFO2,F_OK) == 0) {
		unlink(FIFO2);
	}
	if ((mkfifo(FIFO1,FILE_MODE)) < 0) {
		write(1, "fail to make named pipe\n", 24);
		return 1;
	}
	if ((mkfifo(FIFO2,FILE_MODE)) < 0) {
		write(1, "fail to make named pipe\n", 24);
		return 1;
	}
	if ((readfd = open(FIFO1, O_RDWR)) < 0) {
		write(1, "fail to open named pipe\n", 24);
		return 1;
	}
	
    if ((writefd = open(FIFO2, O_WRONLY)) < 0) {
        write(1, "fail to open named pipe\n", 24);
        return 1;
    }

	/* parent */
	while (1) {
		if ((nread = read(readfd, msg, sizeof(msg))) < 0 ) {
			write(1, "fail to call read()\n", 20);
			return 1;
		}
		if(msg[0] != '\0')
		{
			strcpy(temp_msg, msg);
			// printf("recv: %s\n", msg);
			strcpy(fileNameBuffer, strtok(temp_msg, "\n"));
			actionBuffer = strtok(0, "\n")[0];
			if(actionBuffer == 'r')
				strcpy(byteBuffer, strtok(0, "\n"));
			else if(actionBuffer == 'w')
				strcpy(dataBuffer, strtok(0, "\n"));
			else
				return(-1);
			printf("filename: %s action: %c byte: %s data: %s\n", fileNameBuffer, actionBuffer, byteBuffer, dataBuffer);
			
			if(actionBuffer == 'r')
			{
				n = strlen(fileNameBuffer);
				if((fd=open(fileNameBuffer, O_RDONLY))<0)
				{
					snprintf(fileNameBuffer+n, sizeof(fileNameBuffer)-n, ": can't open, %s\n", strerror(errno));
					n=strlen(fileNameBuffer);
					write(writefd, fileNameBuffer, n);
				}
				else
				{
					while((n=read(fd, dataBuffer, MAXLINE))>0) {
						// dataBuffer[n + 1]='\0';
						write(writefd, dataBuffer, n);
					}
					write(1, "test\n", 5);
					close(fd);
				}
			}
			else if(actionBuffer == 'w')
			{
				if ((fd = open(fileNameBuffer, O_WRONLY)) < 0) {
					write(1, "fail to open file.\n", 19);
					return 1;
				}
				write(fd, dataBuffer, sizeof(char)*strlen(dataBuffer));
				write(writefd, dataBuffer, sizeof(char)*strlen(dataBuffer));
			}
		}
	}
	return 0;
}
