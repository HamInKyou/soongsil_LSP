#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *ssu_thread(void *arg);

int main(void)
{
	pthread_t tid1, tid2;
	int thread1 = 1;
	int thread2 = 2;
	int status;
	
	//쓰레드 생성, 이 쓰레드는 ssu_thread 함수 실행, 인자로 thread1 넘겨줌
	if (pthread_create(&tid1, NULL, ssu_thread, (void *)&thread1) != 0) {
		fprintf(stderr, "pthread_create error\n");
		exit(1);
	}
	
	//쓰레드 생성, 이 쓰레드는 ssu_thread 함수 실행, 인자로 thread2 넘겨줌
	if (pthread_create(&tid2, NULL, ssu_thread, (void *)&thread2) != 0) {
		fprintf(stderr, "pthread_create error\n");
		exit(1);
	}
	
	//첫번째 쓰레드 끝나는 것을 기다림
	pthread_join(tid1, (void *)&status);

	//두번째 쓰레드 끝나는 것을 기다림
	pthread_join(tid2, (void *)&status);
	exit(0);
}

//쓰레드가 실행할 함수
void *ssu_thread(void *arg) {
	int thread_index;
	int i;

	thread_index = *((int *)arg);

	for ( i = 0; i < 5; i ++) {
		printf("%d : %d\n", thread_index, i);
		sleep(1);
	}

	return NULL;
}
