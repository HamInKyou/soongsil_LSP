#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void ssu_signal_handler1(int signo);
void ssu_signal_handler2(int signo);

int main(void)
{
	//SIGINT에 대한 핸들러 등록
	if (signal(SIGINT, ssu_signal_handler1) == SIG_ERR) {
		fprintf(stderr, "cannot handle SIGINT\n");
		exit(EXIT_FAILURE);
	}
	
	//SIGUSR1에 대한 핸들러 등록
	if (signal(SIGUSR1, ssu_signal_handler2) == SIG_ERR) {
		fprintf(stderr, "cannot handle SIGUSR1\n");
		exit(EXIT_FAILURE);
	}

	raise(SIGINT); //현재 프로세스에 SIGINT 시그널 보냄
	raise(SIGUSR1); //현재 프로세스에 SIGUSR1 시그널 보냄
	printf("main return\n");
	exit(0);
}

void ssu_signal_handler1(int signo) { //SIGINT 시그널 받았을때 실행하는 함수
	printf("SIGINT 시그널 발생\n");
}

void ssu_signal_handler2(int signo) { //SIGUSR1 시그널 받았을 때 실행하는 함수
	printf("SIGUSR1 시그널 발생\n");
}
