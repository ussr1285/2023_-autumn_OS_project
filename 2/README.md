# 운영체제 프로젝트 (2)

## 프로그램 설명
통조림을 제조하는 공장을 위한 프로그램입니다. (가상의) 통조림 제조 설비와 직접 연결 된 서버 프로그램을 가동 시킨 후, 인터페이스 역할을 하는 클라이언트 프로그램으로 공장 설비 가동을 시작하거나, 현재까지 생산된 통조림을 확인하거나, 가동을 중단시킬 수 있습니다.
허나 각각의 여러 제조 설비들을 한 프로세스(서버 프로그램) 안에서 쓰레딩을 활용하여 관리하는데, 이 과정에서 발생할 수도 있는 문제를 보여주고, 동기화로 해결하는 것을 보여드리겠습니다.
코드 실행 방법을 먼저 보여드린 후에, 프로그램 상세 정보를 소개하겠습니다.


## 코드 실행 방법 & 프로그램 사용 방법
먼저 실행할 수 있는 server 프로그램과 client 프로그램을 만들기 위해 컴파일을 합니다.
```
make
```

이후 server 프로그램을 먼저 동작시킵니다,
```
./server
```

이후 client 프로그램을 동작시킵니다.
```
./client
```

client 프로그램에서 통조림 음식 멸균기(Autoclave)의 개수와, 통조림 포장기의 개수를 입력하는 상호작용이 차례대로 나옵니다. 1~20 사이의 정수형으로 입력해주시면 됩니다. -> 프로듀서 & 컨슈머 모델에 쓰이는 BUFFER_SIZE를 전역 변수로 설정해 놓느라 이렇게 제한을 두게 되었습니다. 버퍼 사이즈를 임의로 높여서 사용해도 작동합니다. (foodFactory.h 파일의 #define BUFFER_SIZE 20) 
```
가동할 통조림 음식 멸균기의 개수를 입력해주세요: 
가동할 통조림 포장기의 개수를 입력해주세요: 
```

가동할 기계 개수를 입력해주면 Foodcan factory running now. 라는 문구가 나오며, 서버가 기계들을 관리하는 쓰레드들을 가동시키면서 통조림 생산이 시작됩니다.
이후 a, b, c 중에 하나를 입력하여 서버 프로그램을 제어할 수 있습니다.
a를 입력하면 아까 입력했던 가동 중인 기계 갯수가 확인됩니다.
b를 입력하면 현재까지 생산된 통조림 수를 확인할 수 있습니다.
c를 입력하면 생산을 관리하던 서버 프로그램과 클라이언트 프로그램을 중단시킵니다.
```
Foodcan factory running now.
a. 현재 가동 중인 기계 확인.
b. 현재까지 생산된 통조림 수 확인.
c. 기계 동작을 중지.
명령:
```

## 프로그램 상세 설명
### 클라이언트 / 사용자 인터페이스 프로그램 (client.c)
아래 명령들이 가능하며, 실질적인 작동은 모두 서버에서 처리합니다.
대표적인 수행 명령으로는 다음과 같습니다.
- 조리 기계 수 & 포장 기계 수 입력 및 시작.
- 현재 작동 중인 조리 기계 수 & 포장 기계 수 확인.
- 현재까지 생산된 통조림 재고 확인.
- 중지.

아래는 client.c의 핵심 코드입니다. 계속해서 서버(factoryManageServer.c) 프로세스와 통신하며 쿼리를 날려 지시를 하거나 메세지를 받는 업무를 수행합니다.
```c
    while(1) {
        if (firstInput)
        {
            settingInputs(msg, &amountFoodProducer, &packingMachine);
            firstInput = 0;
        }
        else
            controlInput(msg, controlVar);
        if ((nread = write(writefd, msg, sizeof(msg))) < 0 ) { 
            write(1, "fail to call write()\n", 21);
            exit(1);
        }
        read(readfd, readBuffer, sizeof(readBuffer));
        printf("\n%s\n", readBuffer);
        if (controlVar[0] == 'c')
            return 0;
    }
```

### 기계 관리용 서버 프로그램 (factoryManageServer.c)
- 클라이언트로부터 입력된 기계 수 만큼 각각의 생산 쓰레드가 생성되어 동작합니다.
- 프로그램이 종료할 때 아까의 생산 기계를 관리하던 쓰레드들도 같은 프로세스이므로 같이 종료됩니다.

아래는 factoryManageServer.c의 핵심 코드입니다. 클라이언트 프로세스(client.c)와 통신하며 쿼리를 받아 기계 숫자를 처음에 설정하여 쓰레드들(makeCanFood.c)에게 업무를 지시하고, 이후에는 클라이언트로부터의 쿼리에 맞게 데이터를 클라이언트에 보내거나 서버 프로그램을 종료하는 등의 업무를 수행합니다.
```c
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
```


### 서버 프로세스의 쓰레드가 수행하는 업무 소개. (makeCanFood.c)
**음식 멸균 (void* foodProducer(void* arg))**
    - 통조림 재료 통조림 가열하기. (Producer/Consumer 중 Produce 수행.)
**통조림 포장(void* makeFoodCan(void* arg))**
    - 재료 채우기. (Producer/Consumer 중 Consume 수행.)
    - 통조림 포장.
    - 하나의 통조림이 완성되면 기존 재고 수에 방금 만든 통조림 하나를 추가한다. (Read & Write)

이번 프로젝트의 핵심인 동기화가 들어간 부분의 코드들입니다.

먼저 foodProducer는 Producer 역할을 수행하였습니다. 통조림 생산 공장에서 통조림 포장 전, 균을 다 죽여서 보존이 가능하게 만들 수 있도록 하는 역할을 수행합니다. 만약 버퍼(통조림 포장기로 가기 위한)가 가득 차면 기다리고, 아니라면 재료(음식) 멸균을 시작합니다. 그리고 그것을 버퍼에 집어넣어 통조림 포장기로 갈 수 있게 해줍니다. 이 부분을 mutex로 보호하여 멸균된 음식이 버퍼에 초과되지 않도록 만들어줍니다.
```c
void* foodProducer(void* arg) {
    int kPa = 0; // 기압
    int celsiusScale = 0; // 섭씨 온도.

    // 멸균을 위한 Autoclave 세팅 준비.
    while (celsiusScale <= STERILIZATION_CELSIUS_SCALE)
        celsiusScale++;
    while (kPa < STERILIZATION_KPA)
        kPa++;
    while(1) {
        while(sterilizedFoodCount == BUFFER_SIZE) // 버퍼가 가득 찼을 때 대기
            pthread_cond_wait(&buffer_not_full, &mutex); 
        int foodType;
        if (kPa >= STERILIZATION_KPA && celsiusScale >= STERILIZATION_CELSIUS_SCALE)
        {
            foodType = rand() % 100 + 1;
            bufferIn = (bufferIn + 1) % BUFFER_SIZE;
            buffer[bufferIn] = foodType;
            sterilizedFoodCount++;
            // printf("Produced: %d\n", foodType);
        }
        else
        {
            printf("Someting wrong at foodProducer.");
            exit(1);
        }

        pthread_cond_signal(&buffer_not_empty); // 버퍼가 비어있지 않다는 신호
        pthread_mutex_unlock(&mutex); // 뮤텍스 해제

        sleep(1); // 작업 소요 시간.
    }
}
```

makeFoodCan는 consume(통조림 포장) 후 통조림 재고(foodCanCnt)를 증가시켜주는 역할을 수행합니다. 멸균된 재료가 존재할 때, 통에 포장한 후, 통조림 재고 개수를 1 증가시켜 최신화 해주는 역할을 수행합니다.
```c
void* makeFoodCan(void* arg) {
    while(1) {
        pthread_mutex_lock(&mutex); // 뮤텍스 잠금
        
        while(sterilizedFoodCount == 0)
            pthread_cond_wait(&buffer_not_empty, &mutex); // 버퍼가 비었을 때 대기
        int foodType;

        bufferOut = (bufferOut + 1) % BUFFER_SIZE;
        foodType = buffer[bufferOut];
        sterilizedFoodCount--;
        buffer[bufferOut] = 0;
        printf("sterilizedFoodCount: %d\n", sterilizedFoodCount + 1);
        
        // printf(" Consumed: %d\n", foodType);

        foodCanCnt++;
        printf("foodCan: %d\n", foodCanCnt);

        pthread_cond_signal(&buffer_not_full); // 버퍼가 가득 차지 않았다는 신호
        pthread_mutex_unlock(&mutex); // 뮤텍스 해제
        sleep(1); // 작업 소요시간
    }
}
```

