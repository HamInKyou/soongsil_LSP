#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

void *ssu_thread(void *arg);

int main(void)
{
	pthread_t tid;
	pid_t pid;
	
	//새로운 쓰레드 생성, tid에 새로운 쓰레드 아이디 저장된다.
	if (pthread_create(&tid, NULL, ssu_thread, NULL) != 0) {
		fprintf(stderr, "pthread_create error\n");
		exit(1);
	}

	pid = getpid(); //현 프로세스 아이디
	tid = pthread_self(); //현 쓰레드 아이디 (메인 쓰레드)
	printf("Main Thread: pid %u tid %u \n", 
			(unsigned int)pid, (unsigned int) tid);

	sleep(1); 
	exit(0);
}

//새로운 쓰레드가 실행할 함수
void *ssu_thread(void *arg) {
	pthread_t tid;
	pid_t pid;

	pid = getpid(); //현 프로세스 아이디
	tid = pthread_self(); //현 쓰레드 아이디 (새 쓰레드)
	printf("New Thread: pid %d tid %u \n", (int)pid, (unsigned int)tid);
	return NULL;
}
