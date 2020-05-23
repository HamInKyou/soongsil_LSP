#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *ssu_loop1(void *arg);
void *ssu_loop2(void * arg);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //뮤텍스 변수의 초기화
int shared_value; //공유 변수 생성

int main(void)
{
	pthread_t tid1, tid2;
	int status;

	shared_value = 0;
	
	//쓰레드 생성
	if (pthread_create(&tid1, NULL, ssu_loop1, NULL) != 0) {
		fprintf(stderr, "pthread_create error\n");
		exit(1);
	}
	//1초 쉬었다가
	sleep(1);
	
	//쓰레드 생성
	if (pthread_create(&tid2, NULL, ssu_loop2, NULL) != 0) {
		fprintf(stderr, "pthread_create error\n");
		exit(1);
	}

	//쓰레드 1 종료될 때까지 기다린다.
	if (pthread_join(tid1, (void *)&status) != 0) {
		fprintf(stderr, "pthread_join error\n");
		exit(1);
	}
	
	//쓰레드 2 종료될 때까지 기다린다.
	if (pthread_join(tid2, (void *)&status) != 0) {
		fprintf(stderr, "pthread_join error\n");
		exit(1);
	}

	//뮤텍스 변수 해제
	status = pthread_mutex_destroy(&mutex);
	printf("code = %d\n", status);
	printf("programming is end\n");
	exit(0);
}

void *ssu_loop1(void *arg) {
	int i;

	for (i = 0; i < 10; i++){
		pthread_mutex_lock(&mutex); //뮤텍스 변수 잠금
		printf("loop1 : %d\n", shared_value);
		shared_value++; //공유 변수 하나 늘림

		if(i == 10)
			return NULL; 

		pthread_mutex_unlock(&mutex); //뮤텍스 변수 잠금해제
		sleep(1);
	}

	return NULL;
}

void *ssu_loop2(void *arg) {
	int i;

	for (i = 0; i < 10; i++){
		pthread_mutex_lock(&mutex); //뮤텍스 변수 잠금
		printf("loop2 : %d\n", shared_value);
		shared_value++; //공유 변수 하나 늘림

		if(i == 10)
			return NULL;

		pthread_mutex_unlock(&mutex); //뮤텍스 변수 잠금해제
		sleep(1);
	}

	return NULL;
}

