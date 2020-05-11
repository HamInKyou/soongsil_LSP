#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define LINE_MAX 2048

static void ssu_alarm(int signo);

int main(void)
{
	char buf[LINE_MAX];
	int n;
	
	//SIGALRM에 대한 시그널 핸들러 등록
	if(signal(SIGALRM, ssu_alarm) == SIG_ERR) {
		fprintf(stderr, "SIGALRM error\n");
		exit(1);
	}
	alarm(10); //10초 후에 SIGALRM 호출
	
	//표준입력에서 입력 받아 buf에 저장
	if((n = read(STDIN_FILENO, buf, LINE_MAX)) < 0) {
		fprintf(stderr, "read() error\n");
		exit(1);
	}

	alarm(0); //입력 받았을 경우 alarm() 꺼버린다.
	write(STDOUT_FILENO, buf, n); //읽은거 출력
	exit(0);
}

static void ssu_alarm(int signo) {
	//10초가 지나도 입력이 없었을 때 이 프린트문 출력
	printf("ssu_alarm() called!\n");
}
