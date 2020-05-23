#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define VALUE_DONE 10
#define VALUE_STOP1 3
#define VALUE_STOP2 6

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; //뮤텍스 변수 초기화
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; //조건 변수 초기화

void *ssu_thread1(void *arg);
void *ssu_thread2(void *arg);

int glo_val = 0;

int main(void)
{
	pthread_t tid1, tid2; 

	pthread_create(&tid1, NULL, &ssu_thread1, NULL); //쓰레드 생성
	pthread_create(&tid2, NULL, &ssu_thread2, NULL); //쓰레드 생성
	pthread_join(tid1, NULL); //쓰레드 1 끝날 때까지 대기
	pthread_join(tid2, NULL); //쓰레드 2 끝날 때까지 대기

	printf("final value : %d\n", glo_val); 
	exit(0);
}

void *ssu_thread1(void *arg) {
	while(1) {
		pthread_mutex_lock(&lock); //뮤텍스 잠금
		pthread_cond_wait(&cond, &lock); //시그널 기다린다.
		glo_val++; //증가
		printf("global value ssu_thread1: %d\n", glo_val); //출력
		pthread_mutex_unlock(&lock); //뮤텍스 잠금 해제

		if (glo_val >= VALUE_DONE) //10일 경우 끝내기
			return NULL;
	}
}

void *ssu_thread2(void *arg) {
	while(1) {
		pthread_mutex_lock(&lock); //뮤텍스 잠금
		if (glo_val < VALUE_STOP1 || glo_val > VALUE_STOP2) //3보다 작고 6보다 크면
			pthread_cond_signal(&cond); //시그널 보낸다.
		else {
			glo_val++; //증가
			printf("global value ssu_thread2: %d\n", glo_val);// 출력
		}

		pthread_mutex_unlock(&lock); //뮤텍스 잠금 해제

		if (glo_val >= VALUE_DONE)
			return NULL;
	}
}
