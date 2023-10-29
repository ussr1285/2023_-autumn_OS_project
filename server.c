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

	/* parent */
	while (1) {
		if ((nread = read(readfd, msg, sizeof(msg))) < 0 ) {
			write(1, "fail to call read()\n", 20);
			exit(1);
		}
		if(msg[0] != '\0')
		{
			int pid = fork();

			if(pid == 0) // child
			{
				strcpy(temp_msg, msg);
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
						write(writefd, "can't open file.\n", 17);
						continue ;
					}
					else
					{
						while((n=read(fd, dataBuffer, MAXLINE))>0) {
							write(writefd, dataBuffer, atoi(byteBuffer));
						}
						close(fd);
					}
				}
				else if(actionBuffer == 'w')
				{
					if ((fd = open(fileNameBuffer, O_WRONLY | O_TRUNC)) < 0) {
						write(1, "fail to open file.\n", 19);
						write(writefd, "fail to open file.\n", 19);
						continue ;
					}
					write(fd, dataBuffer, sizeof(char)*strlen(dataBuffer));
					write(writefd, dataBuffer, sizeof(char)*strlen(dataBuffer));
					close(fd);
				}
			} 
			else if(pid < 0)// error
			{
				write(1, "Can't make child process.\n", 26);
				write(writefd, "Server Error.\n", 14);
			}
		}
	}
	return 0;
}
