# 운영체제 2023 가을 프로젝트 1

# 프로그램 실행 방법

먼저 아래와 같이 수행하여 실행할 수 있는 파일로 만듭니다.
```{bash}
cc server.c -o server
cc client.c -o client
```

이후 먼저 다음 명령어로 서버를 동작시킵니다. (클라이언트와 같은 터미널에서 백그라운드 프로세스로 진행하는 것은 권장드리지 않습니다. 그렇게 하면 클라이언트와 통신할 때, 서버에서 자체적으로 클라이언트에서 온 메시지가 동일한 터미널에 출력되기 때문입니다.)
```{bash}
./server
```

이후 다른 터미널에서 다음 명령어로 클라이언트를 동작시킵니다. (단, 서버 동작시에만 정상 작동 됩니다.)
```
./client
```

프로그램을 실행시키면 여러 입력 칸이 나옵니다.
파일 이름 정보를 입력하고, read 할 것인지 write할 것인지에 따라 r 혹은 w를 입력합니다.
만약 w를 입력했었다면 파일에 쓸 텍스트를 입력해야하고, 만약 r을 입력했었다면 읽을 바이트 수를 입력하면 됩니다.
(전부 한 줄씩만 입력 가능합니다.)
```{c}
Please enter file name: test.txt
Please enter access type [r or w]: w
Please enter data to write: gda
3
Please enter file name: test.txt
Please enter access type [r or w]: r
Please enter bytes: 2
gd
```
프로그램 실행 방법은 이상입니다.


# 코드 설명

과제와 관련된 핵심 내용 위주로 설명 드리겠습니다.

## server.c

### 전처리
named pipe를 구현하고 입출력을 활용하고, 그 과정 속에 문자열 처리 등을 위해 다음과 같은 헤더파일을 사용하였습니다.

```{c}
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
```

named pipe를 활용할 것이기 때문에, 임의의 FIFO1(/tmp/fifo.1)과 FIFO2(/tmp/fifo.2)를 사용했습니다.
```{c}
#define FIFO1 "/tmp/fifo.1"
#define FIFO2 "/tmp/fifo.2"
```

### int main(void)
먼저 FIFO1과 FIFO2를 지금부터 사용하기 위해 초기화 해주고, mkfifo를 이용하여 FIFO1과 FIFO2라는 named pipe를 생성해주었습니다.
이후 FIFO1과 FIFO2에 대한 파일 디스크립터를 각각 readfd(FIFO1)와 writefd(FIFO2)에 생성해주었습니다.
```{c}
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
```

이후 while에서 계속 반복을 수행하면서, readfd(FIFO1)에서 클라이언트로부터 데이터를 읽어온다면 계속 코드를 실행합니다.
이때 fork를 사용하여 자식 프로세스를 실행시킵니다. 자식 프로세스가 동작하는 코드는 조금 이따가 설명 드리도록 하겠습니다.
부모 프로세스는 concurrency를 위하여 waitpid 함수에 WNOHANG 옵션을 주어서 자식 프로세스를 따로 기다리지 않도록 하였습니다.
만약 child process를 만들 수 없는 상황이라면, client에게 "Server Error." 라는 메시지만 보내도록 하였습니다.

```{c}
	while (1) {
		if ((nread = read(readfd, msg, sizeof(msg))) < 0 ) {
			write(1, "fail to call read()\n", 20);
			exit(1);
		}
		if(msg[0] != '\0')
		{
			childpid = fork();

			if(childpid == 0) // child
			{
				// 자식 프로세스 실행 코드....
			}
			else if(childpid > 0) // parent
			{
				waitpid(childpid, NULL, WNOHANG); 
			}
			else if(childpid < 0) // error
			{
				write(1, "Can't make child process.\n", 26);
				write(writefd, "Server Error.\n", 14);
			}
		}
	}
```

아까 설명을 보류했던 자식 프로세스가 동작할 코드입니다.
설명에 앞서 요약하자면 실질적으로 자식 프로세스에서 클라이언트에서의 요청을 처리합니다.
클라이언트에서의 요청은 한 문자열로 한방에 오기 때문에, 그 문자열을 '\n'을 기준으로 구분하여, 파일 이름(fileNameBuffer)과 수행 방법(actionBuffer)을 먼저 얻습니다. 
이후 수행 방법에 따라서 r이라면 바이트(byteBuffer), w라면 데이터(dataBuffer)를 알맞은 변수에 할당하고 각각 수행방법에 따른 처리를 합니다.
r의 경우에는 특정 파일을 read해달라는 클라이언트의 요청이므로, 클라이언트가 보내줬던 파일 이름과 동일한 이름의 파일을 읽어서, 요청한 byte만큼만 writefd(FIFO2)로 write하여 클라이언트에게 보내줍니다. 만약 해당하는 파일이 없으면 파일을 열수 없다는 메세지를 보내줍니다.
w의 경우에는 특정 파일에 클라이언트가 입력한 데이터를 write달라는 요청이었으므로, 클라이언트가 보내준 데이터를 해당하는 파일에 write 합니다. 그리고 그 데이터에 관한 byte를 클라이언트에게 반환해줍니다.


```{c}
/...(생략)...

if(childpid == 0) // child
{
	strcpy(temp_msg, msg);
	strcpy(fileNameBuffer, strtok(temp_msg, "\n"));
	actionBuffer = strtok(0, "\n")[0];
	if(actionBuffer == 'r')
		strcpy(byteBuffer, strtok(0, "\n"));
	else if(actionBuffer == 'w')
		strcpy(dataBuffer, strtok(0, "\n"));
	else
		_exit(1);
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
		if ((fd = open(fileNameBuffer, O_WRONLY | O_TRUNC)) < 0)
		{
			write(1, "fail to open file.\n", 19);
			write(writefd, "fail to open file.\n", 19);
			continue ;
		}
		write(fd, dataBuffer, sizeof(char)*strlen(dataBuffer));
		lenBuffer[sizeof(char)*strlen(dataBuffer)] = '\0';
		sprintf(lenBuffer, "%lu", sizeof(char)*strlen(dataBuffer));
		write(writefd, lenBuffer, strlen(dataBuffer));
		close(fd);
	}
	_exit(0);
}
/...(생략)...
```

이상 server.c의 코드 설명이었습니다.


## client.c


### 전처리
named pipe를 구현하고 입출력을 활용하고, 그 과정 속에 문자열 처리 등을 위해 다음과 같은 헤더파일을 사용하였습니다.

```{c}
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
```

named pipe를 활용할 것이기 때문에, 임의의 FIFO1(/tmp/fifo.1)과 FIFO2(/tmp/fifo.2)를 사용했습니다.
```{c}
#define FIFO1 "/tmp/fifo.1"
#define FIFO2 "/tmp/fifo.2"
```

### int main(void)

FIFO1과 FIFO2에 대한 파일 디스크립터를 각각 readfd(FIFO2)와 writefd(FIFO1)에 생성해주었습니다.
```{c}
if ((readfd = open(FIFO2, O_RDWR)) < 0) {
	write(1, "fail to open named pipe\n", 24);
	exit(1);
}
if ((writefd = open(FIFO1, O_WRONLY)) < 0) {
	write(1, "fail to open named pipe\n", 24);
	exit(1);
}
```

userInputs 이라는 자체 제작한 함수를 사용해 사용자의 입력을 받습니다.
이후 writefd(FIFO1)에 사용자의 입력을 '\n'을 구분자로 하여 합쳐진 msg을 출력하여, server로 데이터를 보냅니다.
서버로부터 응답이 돌아오면 그 응답을 읽어서 클라이언트의 터미널에 출력하여 확인합니다.
```{c}
while(1) {
	userInputs(fileNameBuffer, msg, &actionBuffer, byteBuffer, dataBuffer, readBuffer, readfd, writefd);

	if ((nread = write(writefd, msg, sizeof(msg))) < 0 ) { 
		write(1, "fail to call write()\n", 21);
		exit(1);
	}
	readBuffer[atoi(byteBuffer)] = '\0';
	read(readfd, readBuffer, sizeof(readBuffer));
	printf("%s\n", readBuffer);
}
```

### int userInputs(char *fileNameBuffer, char *msg, char *actionBuffer, char *byteBuffer, char *dataBuffer, char *readBuffer, int readfd, int writefd)

이 함수는 사용자의 입력을 안내 문구와 함께 하나씩 받습니다.
파일 이름을 입력받고, access type을 입력 받습니다.
이후 입력 받았던 access type이 r이면 byte를 입력받고 w라면 입력할 데이터를 입력받습니다.
각 입력을 받을 때마다 msg에 합쳐 놓아서, 서버로 보낼 메시지를 만듭니다.

단, 이때 access type이 r 이나 w 중 하나가 아니라면, 오류 메시지를 띄우고 프로그램을 종료시킵니다.

```{c}
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
{
	write(1, "Please enter access correctly.\n", 31);
	exit(1);
}

```
