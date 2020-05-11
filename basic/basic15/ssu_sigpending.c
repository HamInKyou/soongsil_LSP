#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(void)
{
	sigset_t pendingset; //sigset 선언
	sigset_t sig_set; //sigset 선언
	int count = 0;

	sigfillset(&sig_set); //sigset sig_set의 시그널을 모두 켜준다.
	sigprocmask(SIG_SETMASK, &sig_set, NULL); //모든 시그널을 블락시켜준다.

	while (1) {
		printf("count: %d\n", count++);
		sleep(1);

		if(sigpending(&pendingset) == 0) { //블록중에 발생한 시그널 집합을 불러옴
			if(sigismember(&pendingset, SIGINT)) { //블록중에 SIGINT가 발생했었다면
				printf("SIGINT가 블록되어 대기 중. 무한 루프를 종료.\n");
				break;
			}
		}
	}
	exit(0);
}
