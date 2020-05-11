#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void ssu_alarm(int signo);

int main(void)
{
	printf("Alarm Setting\n");
	signal(SIGALRM, ssu_alarm); //알람 시그널에 대한 핸들러 세팅
	alarm(2); //2초 후에 SIGALRM 울림

	while(1) {
		printf("done\n"); 
		pause(); //시그널 들어올때까지 멈춤
		alarm(2); //핸들러 실행하고 나서 다시 2초후에 SIGALRM 울리게
	}

	exit(0);
}

void ssu_alarm(int signo) {
	printf("alarm..!!!\n"); //시그널 들어오면 이거 출력
}
