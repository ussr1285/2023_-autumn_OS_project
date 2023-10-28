#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 4096
#define MSG_SIZE 10000
#define FIFO1 "/tmp/fifo.1"
#define FIFO2 "/tmp/fifo.2"

int userInputs(char *fileNameBuffer, char *msg, char *actionBuffer, char *byteBuffer, char *dataBuffer, char *readBuffer, int readfd, int writefd);

int main(void) {
    char msg[MSG_SIZE];
    int readfd;
    int writefd;
    int nread;
    char fileNameBuffer[MAXLINE];
    char actionBuffer;
    char byteBuffer[10];
	char dataBuffer[MAXLINE];
    char readBuffer[MAXLINE];

	if ((readfd = open(FIFO2, O_RDWR)) < 0) {
		write(1, "fail to open named pipe\n", 24);
		return 1;
	}
    if ((writefd = open(FIFO1, O_WRONLY)) < 0) {
        write(1, "fail to open named pipe\n", 24);
        return 1;
    }

    while(1) {
        userInputs(fileNameBuffer, msg, &actionBuffer, byteBuffer, dataBuffer, readBuffer, readfd, writefd);

        if ((nread = write(writefd, msg, sizeof(msg))) < 0 ) { 
            write(1, "fail to call write()\n", 21);
            return 1;
        }
        read(readfd, readBuffer, sizeof(readBuffer));
        printf("%s\n", readBuffer);
    }
    return 0;
}

int userInputs(char *fileNameBuffer, char *msg, char *actionBuffer, char *byteBuffer, char *dataBuffer, char *readBuffer, int readfd, int writefd)
{
    write(1, "Please enter file name: ", 24);
    fgets(fileNameBuffer, MAXLINE, stdin);
	strcpy(msg, fileNameBuffer);

    write(1, "Please enter access type [r or w]: ", 35);
    fgets(actionBuffer, 2, stdin);
	while (getchar() != '\n');
	strcat(msg, actionBuffer);
    strcat(msg, "\n");

	if (*actionBuffer == 'r')
	{
		write(1, "Please enter bytes: ", 20);
    	fgets(byteBuffer, 11, stdin);
		strcat(msg, byteBuffer);
	}
	else if (*actionBuffer == 'w')
	{
		write(1, "Please enter data to write: ", 28);
    	fgets(dataBuffer, MAXLINE, stdin);
		strcat(msg, dataBuffer);
	}
	else
		return (1); // 오류 메시지도 띄워야 할 듯.
	return (0);
}
