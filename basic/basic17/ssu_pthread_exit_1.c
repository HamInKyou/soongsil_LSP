#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define THREAD_NUM 5

void *ssu_printhello(void *arg);

int main(void)
{
	pthread_t tid[THREAD_NUM];
	int i;

	//쓰레드 5개 만들기 위한 반복
	for (i = 0; i < THREAD_NUM; i++) {
		printf("In main: creating thread %d\n", i);
		
		//쓰레드 생성, 쓰레드는 ssu_printhello 실행, 인자로 i값 넣어줌
		if (pthread_create(&tid[i], NULL, ssu_printhello, (void *)&i) != 0)
		{
			fprintf(stderr, "pthread_create error\n");
			exit(1);
		}
	}
	
	//메인 쓰레드 종료
	pthread_exit(NULL);
	exit(0);
}

void *ssu_printhello(void *arg) {
	int thread_index;
	

	thread_index = *((int *)arg);
	printf("Hello World! It's me, thread #%d!\n", thread_index);
	pthread_exit(NULL); //새로 만든 현 쓰레드 종료
	return NULL;
}
