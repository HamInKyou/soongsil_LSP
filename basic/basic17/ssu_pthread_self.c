#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *ssu_thread(void *arg);
int main(void)
{
	pthread_t tid;
	
	//쓰레드 생성, ssu_thread 실행
	if (pthread_create(&tid, NULL, ssu_thread, NULL) != 0) {
		fprintf(stderr, "pthread_create error\n");
	}
	
	//새로 생성한 쓰레드 아이디 출력
	printf("%u\n", (unsigned int)tid);

	//쓰레드 생성, ssu_thread 실행
	if (pthread_create(&tid, NULL, ssu_thread, NULL) != 0) {
		fprintf(stderr, "pthread_create error\n");
		exit(1);
	}
	
	//새로 생성한 쓰레드 아이디 출력
	printf("%u\n", (unsigned int)tid);
	sleep(1); //1초 휴식
	exit(0);
}

void *ssu_thread(void *arg) {
	pthread_t tid;

	tid = pthread_self(); //현재 쓰레드 아이디 tid에 저장
	printf("->%u\n", (unsigned int)tid); 
	return NULL;
}
