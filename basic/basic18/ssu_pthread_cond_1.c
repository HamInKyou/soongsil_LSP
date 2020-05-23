#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *ssu_thread1(void *arg);
void *ssu_thread2(void *arg);

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER; //뮤텍스 변수 초기화
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER; //뮤텍스 변수 초기화
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER; //조건 변수 초기화
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER; //조건 변수 초기화

int count = 0;
int input = 0;
int t1 = 0, t2 = 0;

int main(void)
{
	pthread_t tid1, tid2;
	int status;
	
	//쓰레드 생성
	if (pthread_create(&tid1, NULL, ssu_thread1, NULL) != 0) {
		fprintf(stderr, "pthread_create error\n");
		exit(1);
	}
	
	//쓰레드 생성
	if (pthread_create(&tid2, NULL, ssu_thread2, NULL) != 0) {
		fprintf(stderr, "pthread_create error\n");
		exit(1);
	}

	while (1) {
		printf("2개 이상의 개수 입력 : ");
		scanf("%d", &input);

		if(input >= 2) {
			pthread_cond_signal(&cond1); //cond1 기다리는 쓰레드1에 시그널 보냄
			break;
		}
	}
	
	pthread_join(tid1, (void *)&status); //쓰레드1 종료될 때까지 대기
	pthread_join(tid2, (void *)&status); //쓰레드2 종료될 때까지 대기

	printf("complete \n");
	exit(0);
}

void *ssu_thread1(void *arg) {
	while (1) {
		pthread_mutex_lock(&mutex1); //뮤텍스 변수 잠금

		if (input < 2)
			pthread_cond_wait(&cond1, &mutex1); //cond1 시그널을 기다린다.

		if (input == count) { //카운트가 입력한 값이랑 같아졌을 때
			pthread_cond_signal(&cond2); //쓰레드2에 시그널을 보낸다.
			break;
		}

		if (count == 0) { //맨 처음 시작할 때
			t2++; //t2를 키운다.
			count++; //카운트 하나 늘린다.
			printf("Thread 1 : %d\n", t1); //t1을 출력해준다.(0)
		}
		else if (count % 2 == 0) { //카운트가 짝수번일 때
			t1 += t2; //t1에 t2를 더해준다.
			count++; //카운트를 늘린다.
			printf("Thread 1 : %d\n", t1);
		}

		pthread_cond_signal(&cond2); //쓰레드2에 시그널 보낸다.
		pthread_cond_wait(&cond1, &mutex1); //cond1 시그널을 기다린다.
		pthread_mutex_unlock(&mutex1); //뮤텍스 잠금 해제
	}

	return NULL;
}

void *ssu_thread2(void *arg) {
	while (1) {
		pthread_mutex_lock(&mutex2); //뮤텍스 변수 잠금

		if (input < 2)
			pthread_cond_wait(&cond2, &mutex2); //시그널을 기다린다.

		if (input == count) { //카운트가 입력한 값이랑 같으면
			pthread_cond_signal(&cond1); //쓰레드 1에 시그널을 보낸다.
			break;
		}

		if (count == 1) { //두번째 카운트일 경우
			count++; //카운트 늘리고
			printf("Thread 2 : %d\n", t2); //t2 출력한다.
		}
		else if (count % 2 == 1) { //홀수번째 카운트일 경우
			t2 += t1; //t2에 t1을 더해준다.
			count++; //카운트를 늘려준다.
			printf("Thread 2 : %d\n", t2); //t2 출력한다.
		}

		pthread_cond_signal(&cond1); //쓰레드1에 시그널 보낸다.
		pthread_cond_wait(&cond2, &mutex2); //시그널을 받을 때까지 대기한다.
		pthread_mutex_unlock(&mutex2); //뮤텍스 변수 잠금 해제
	}

	return NULL;
}
