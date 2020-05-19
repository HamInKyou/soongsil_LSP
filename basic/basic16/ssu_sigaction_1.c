#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void ssu_signal_handler(int signo) {
	printf("ssu_signal_handler control\n");
}

int main(void) {
	struct sigaction sig_act;
	sigset_t sig_set;

	sigemptyset(&sig_act.sa_mask); //sigaction 구조체의 sa_mask를 다 비워준다.
	sig_act.sa_flags = 0; //시그널 옵션을 0으로 해준다.
	sig_act.sa_handler = ssu_signal_handler; //시그널 핸들러를 등록해준다.
	sigaction(SIGUSR1, &sig_act, NULL); //SIGUSR1 시그널 핸들러를 등록해준다.
	printf("before first kill()\n");
	kill(getpid(), SIGUSR1); //현 프로세스에 SIGUSR1 시그널 보낸다.
	sigemptyset(&sig_set); //sig_set을 다 비워준다.
	sigaddset(&sig_set, SIGUSR1); //SIGUSR1을 sig_set에 추가해준다.
	sigprocmask(SIG_SETMASK, &sig_set, NULL); //sig_set에 있는 것들 블록시켜준다.
	printf("before second kill()\n");
	kill(getpid(), SIGUSR1);
	printf("after second kill()\n");
	exit(0);
}
