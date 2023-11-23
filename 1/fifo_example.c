/*
프로그램 내용:

1. 클라이언트-서버 구조를 기반으로 하고, named pipe를 통하여 프로세스간 통신수행 (서버에서 named pipe를 생성하고 (mkfifo), 클라이언트와 서버에서 이것을 open)
2. 클라이언트는 사용자로 부터 파일명, 파일억세스타입 (r(읽기) 또는 w(쓰기)), 읽을 bytes 수(읽기의 경우), 데이터 스트링(쓰기의 경우)를 입력받음
3. 클라이언트는 named pipe 를 통하여 서버에게 파일 억세스 요청
4. 서버는 클라이언트로부터 named pipe 통해 파일 억세스 요청을 받은 후, 자신이 직접 수행하는 것이 아니라 concurrency를 위하여 child를 fork하여 child에게 파일 억세스 를 수행하고 응답하게 함 (읽기의 경우 데이터 스트링을 응답하고, 쓰기의 경우 쓰기를 한 bytes 수를 응답). 응답을 완료한 child는 종료됨.
5. 2-4의 과정을 반복하여 수행함. 6. 구체적인 사용자 인터페이스 방법과 프로세스간 교신 데이터 형식은 자율적으로 정함. * “pipe 사용법” 첨부화일의 내용을 참고바람: 과제에서 요구되는 구체적인 구조와는 다르지만, 일반적인 system call 사용과 client-server 모델은 참고 가능. ** 작업환경은 Linux에서 C 언어를 사용함
*/

/*
int mkfifo(const char *pathname, mode_t mode);
(1) pathname은 정상적인 유닉스의 경로명이다. 이것은 FIFO의 이름을 나타낸다.
(2) mode는 파일 퍼미션 비트를 나타낸다. open()의 두번째 파라미터와 비슷하다.
(3) mkfifo()는 O_CREAT | O_EXCL을 의미한다. 그것은 새로운 FIFO를 만들거나 이
미 FIFO가 존재한다면 EEXIT의 에러를 리턴한다.
(4) 새로운 FIFO를 생성하고 싶지 않다면, mkfifo() 대신에 open()을 호출한다. 
쓰기 모드로 pipe나 FIFO를 열면, 항상 데이터를 뒤에서 추가하게 되고, 읽기모드로 연다
면, 항상 pipe나 FIFO의 시작지점에서 읽게 된다. 그래서 pipe나 FIFO에 대해 lseek()를
호출한다면, ESPIPE 에러가 리턴 된다.
*/

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

void client(int, int), server(int, int);

int main()
{
    int readfd, writefd;
    pid_t childpid;

    if ((mkfifo(FIFO1, FILE_MODE) < 0) && (errno != EEXIST))
    {
        printf("can't create %s", FIFO1);
        return(1);
    }
    if ((mkfifo(FIFO2, FILE_MODE) < 0) && (errno != EEXIST))
    {
        unlink(FIFO1);
        printf("can't create %s", FIFO2);
        return(1);
    }
    if ((childpid=fork())==0)
    { /* child */
        readfd = open(FIFO1, O_RDONLY, 0);
        writefd = open(FIFO2, O_WRONLY, 0);

        server(readfd, writefd);
        return(0);
    }

    /* parent */
    writefd = open(FIFO1, O_WRONLY, 0);
    readfd = open(FIFO2, O_RDONLY, 0);

    client(readfd, writefd);

    waitpid(childpid, NULL, 0); /* wait for child to terminate */

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
    char buff[MAXLINE];

    /* read pathname */
    fgets(buff, MAXLINE, stdin);
    len = strlen(buff);
    if(buff[len-1] == '\n')
        len--;

    write(writefd, buff, len);

    while((n=read(readfd, buff, MAXLINE))>0)
        write(STDOUT_FILENO, buff, n);
}

void server(int readfd, int writefd)
{
    int fd;
    size_t n;
    char buff[MAXLINE+1];

    if((n=read(readfd, buff, MAXLINE))==0)
    {
        printf("end-of-file");
        return ;
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
}
