#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAXLINE 4096
#define STDOUT_FILENO 1
#define FIFO1 "/tmp/fifo.1"
#define FIFO2 "/tmp/fifo.2"
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

void client(int readfd, int writefd);
int	parse_to_int(char *str);

int main()
{
    int readfd, writefd;
    // pid_t childpid;

    /* parent */
    readfd = open(FIFO2, O_RDONLY, 0);
    writefd = open(FIFO1, O_WRONLY, 0);

    client(readfd, writefd);

    close(readfd);
    close(writefd);

    unlink(FIFO1);
    unlink(FIFO2);
    return(0);
}

void client(int readfd, int writefd)
{
    size_t len;
    size_t n;
    char fileNameBuffer[MAXLINE];
    char actionBuffer;
    char byteBuffer[10];
    int byte = 0;

    /* read pathname */
    
    write(1, "Please enter file name: ", 24);
    fgets(fileNameBuffer, MAXLINE, stdin);
    len = strlen(fileNameBuffer);
    if(fileNameBuffer[len-1] == '\n')
        len--;
    write(1, "Please enter access type [r or w]: ", 35);
    fgets(&actionBuffer, 1, stdin);
    write(1, "Please enter bytes: ", 20);
    fgets(byteBuffer, 10, stdin);
    byte = parse_to_int(byteBuffer);
    // write(writefd, Buff, len);

    while((n=read(readfd, fileNameBuffer, MAXLINE))>0)
        write(STDOUT_FILENO, fileNameBuffer, n);
}

int	parse_to_int(char *str)
{
	int	result;
	int	ten_size;
	int	i;
    int end_i = strlen(str);

	result = 0;
	ten_size = 1;
	i = end_i;
	while (i >= 0)
	{
		result += (str[i] - '0') * ten_size;
		ten_size *= 10;
		i--;
	}
	return (result);
}

