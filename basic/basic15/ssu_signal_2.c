#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static void ssu_signal_handler(int signo);

int main(void)
{
	//SIGINT에 대한 시그널 핸들러 등록
	if (signal(SIGINT, ssu_signal_handler) == SIG_ERR) {
		fprintf(stderr, "cannot handle SIGINT\n");
		exit(EXIT_FAILURE);
	}
	
	//SIGTERM에 대한 시그널 핸들러 등록
	if (signal(SIGTERM, ssu_signal_handler) == SIG_ERR) {
		fprintf(stderr, "cannot handle SIGTERM\n");
		exit(EXIT_FAILURE);
	}
	
	//SIGPROF에 대한 시그널 핸들러를 디폴트로 등록
	if (signal(SIGPROF, SIG_DFL) == SIG_ERR) {
		fprintf(stderr, "cannot reset SIGPROF\n");
		exit(EXIT_FAILURE);
	}
	
	//SIGHUP 시그널을 무시하겠다.
	if (signal(SIGHUP, SIG_IGN) == SIG_ERR) {
		fprintf(stderr, "cannot ignore SIGHUP\n");
		exit(EXIT_FAILURE);
	}

	while(1)
		pause(); //계속 멈춰있는다.

	exit(0);
}

static void ssu_signal_handler(int signo) {
	if (signo == SIGINT) //SIGINT 받았을 경우 아래 프린트문 실행
		printf("caught SIGINT\n");
	else if (signo == SIGTERM) //SIGTERM 받았을 경우 아래 프린트문 실행
		printf("caught SIGTERM\n");
	else { //다른 핸들러로 설정하지 않은 시그널이 들어왔을 경우
		fprintf(stderr, "unexpected signal\n");
		exit(EXIT_FAILURE); //끝내기
	}
	exit(EXIT_SUCCESS);
}
