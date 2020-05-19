#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *ssu_thread(void *arg);

pthread_t glo_tid;

int main(void)
{
	pthread_t loc_tid;
	
	//쓰레드 생성, loc_tid에 쓰레드 아이디 저장, 쓰레드는 ssu_thread 실행
	if (pthread_create(&loc_tid, NULL, ssu_thread, NULL) != 0) {
		fprintf(stderr, "pthread_create error\n");
		exit(1);
	}

	sleep(5); //메인 쓰레드 5초 휴식

	if (pthread_equal(loc_tid, glo_tid) == 0) { //같은 쓰레드 id를 갖는지?
		printf("다른 쓰레드\n");
		exit(0);
	}

	printf("동일한 쓰레드\n");
	exit(0);
}

void *ssu_thread(void *arg) {
	printf("쓰레드에서 자신의 쓰레드 ID를 전역변수에 할당 \n");
	glo_tid = pthread_self(); //전역 쓰레드에 자기 쓰레드 아이디 할당
	return NULL;
}
