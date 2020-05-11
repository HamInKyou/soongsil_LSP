#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void ssu_signal_handler(int signo);

int count = 0;

int main(void)
{
	//SIGALRM에 대한 시그널 핸들러 등록
	signal(SIGALRM, ssu_signal_handler);
	alarm(1); //1초 후에 SIGALRM 시그널 호출

	while(1); //계속 반복

	exit(0);
}

void ssu_signal_handler(int signo) {
	printf("alarm %d\n", count++); //1초마다 count 늘려가면서 출력
	alarm(1); //다시 1초 후에 SIGALRM 시그널 호출하게
}
