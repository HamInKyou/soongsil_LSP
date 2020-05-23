#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; //뮤텍스 변수 초기화
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; //조건 변수 초기화

int glo_val1 = 1, glo_val2 = 2; 

void *ssu_thread1(void *arg);
void *ssu_thread2(void *arg);

int main(void)
{
	pthread_t tid1, tid2;

	pthread_create(&tid1, NULL, ssu_thread1, NULL); //쓰레드 생성
	pthread_create(&tid2, NULL, ssu_thread2, NULL); //쓰레드 생성
	pthread_join(tid1, NULL); //쓰레드 1 끝날때까지 대기
	pthread_join(tid2, NULL); //쓰레드 2 끝날때까지 대기
	pthread_mutex_destroy(&lock); //뮤텍스 변수 해제
	pthread_cond_destroy(&cond); //조건 변수 해제

	exit(0);
}

void *ssu_thread1(void *arg) {
	sleep(1); //1초 휴식
	glo_val1 = 2; 
	glo_val2 = 1;

	if (glo_val1 > glo_val2) 
		pthread_cond_broadcast(&cond); //시그널을 기다리는 모든 쓰레드에 보냄

	printf("ssu_thread1 end\n"); 
	return NULL;
}

void *ssu_thread2(void *arg) {
	struct timespec timeout;
	struct timeval now;

	pthread_mutex_lock(&lock); //뮤텍스 변수 잠금
	gettimeofday(&now, NULL); //현재 시간 저장

	timeout.tv_sec = now.tv_sec + 5; 
	timeout.tv_nsec = now.tv_usec * 1000;

	if (glo_val1 <= glo_val2) { //glo_val1이 아직 glo_val2보다 작다는건 쓰레드 1 쉬는중
		printf("ssu_thread2 sleep\n"); 
		//쓰레드2 timeout 때까지 대기상태
		if (pthread_cond_timedwait(&cond, &lock, &timeout) == ETIMEDOUT)
			printf("timeput\n");
		else //5초 지나기전에 쓰레드 1에서 시그널 받아서 위의 조건문 리턴안된다.
			printf("glo_val1 = %d, glo_val2 = %d\n", glo_val1, glo_val2);
	}

	pthread_mutex_unlock(&lock); //뮤텍스 잠금 해제
	printf("ssu_thread2 end\n");
	return NULL;
}
