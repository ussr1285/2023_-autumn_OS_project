#include <stdio.h> // test용. 
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXLINE 4096
#define STDOUT_FILENO 1
#define FIFO1 "/tmp/fifo.1"
#define FIFO2 "/tmp/fifo.2"
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

void client(char* readFilePath, char* writeFilePath);
int server(char* readFilePath, char* writeFilePath);

int main()
{
    pid_t childpid;
	int serverErrCheck;

    if ((childpid=fork())==0)
    { /* child */
        serverErrCheck = server(FIFO1, FIFO2);
		if(serverErrCheck != 0)
			return (1);
		return(0);
    }
	else
	{ /* parent */
		client(FIFO2, FIFO1);

		waitpid(childpid, NULL, 0); /* wait for child to terminate */
		// 닫는거 어디에 넣어야 할 듯.
		unlink(FIFO1);
		unlink(FIFO2);
	}
    return(0);
}

void client(char* readFilePath, char* writeFilePath)
{
    size_t len;
    size_t n;
	int readfd;
	int writefd;
    char buff[MAXLINE];

	printf("readstart!\n");
    /* read pathname */
    fgets(buff, MAXLINE, stdin);
	printf("readend!\n");
    len = strlen(buff);
    if(buff[len-1] == '\n')
        len--;

	printf("fdstart!\n");
	readfd = open(readFilePath, O_RDONLY, 0);
	writefd = open(writeFilePath, O_WRONLY, 0);
	printf("fdend!!\n");	

    write(writefd, buff, len);

    while((n=read(readfd, buff, MAXLINE))>0)
        write(STDOUT_FILENO, buff, n);
		// close(readfd);
		// close(writefd);
}

int server(char* readFilePath, char* writeFilePath)
{
    int fd;
    size_t n;
    char buff[MAXLINE+1];
	int readfd;
	int writefd;

	if ((mkfifo(readFilePath, FILE_MODE) < 0) && (errno != EEXIST))
    {
        printf("can't create %s", readFilePath);
        return (1);
    }
    if ((mkfifo(writeFilePath, FILE_MODE) < 0) && (errno != EEXIST))
    {
        unlink(readFilePath);
        printf("can't create %s", writeFilePath);
        return (1);
    }

    readfd = open(readFilePath, O_RDONLY, 0);
    writefd = open(writeFilePath, O_WRONLY, 0);

    if((n=read(readfd, buff, MAXLINE))==0)
    {
        printf("end-of-file");
        return (0);
    }
    buff[n]='\0';

    if((fd=open(buff,O_RDONLY))<0)
    {
        snprintf(buff+n, sizeof(buff)-n, ": can't open, %s\n", strerror(errno));
        n=strlen(buff);
        write(writefd, buff, n);
    }
    else
    {
        while((n=read(fd, buff, MAXLINE))>0)
            write(writefd, buff, n);
        close(fd);
    }

	return (0);
}
