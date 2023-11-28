#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXLINE 4096
#define MSG_SIZE 10000
#define FIFO1 "/tmp/fifo.1"
#define FIFO2 "/tmp/fifo.2"

void userInputs(char *fileNameBuffer, char *msg, char *actionBuffer, char *byteBuffer, char *dataBuffer);

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
		exit(1);
	}
    if ((writefd = open(FIFO1, O_WRONLY)) < 0) {
        write(1, "fail to open named pipe\n", 24);
        exit(1);
    }

    while(1) {
        userInputs(fileNameBuffer, msg, &actionBuffer, byteBuffer, dataBuffer);

        if ((nread = write(writefd, msg, sizeof(msg))) < 0 ) { 
            write(1, "fail to call write()\n", 21);
            exit(1);
        }
        readBuffer[atoi(byteBuffer)] = '\0';
        read(readfd, readBuffer, sizeof(readBuffer));
        printf("%s\n", readBuffer);
    }
    return 0;
}

void userInputs(char *fileNameBuffer, char *msg, char *actionBuffer, char *byteBuffer, char *dataBuffer)
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
    	if(isdigit(*byteBuffer) == 0)
		{
			write(1, "Please enter a number.\n", 23);
			exit(1);
		}
		strcat(msg, byteBuffer);
	}
	else if (*actionBuffer == 'w')
	{
		write(1, "Please enter data to write: ", 28);
    	fgets(dataBuffer, MAXLINE, stdin);
		strcat(msg, dataBuffer);
	}
	else
    {
        write(1, "Please enter access correctly.\n", 31);
        exit(1);
    }
	return ;
}
