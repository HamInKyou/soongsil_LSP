#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(void)
{
	sigset_t sig_set; //sigset 선언
	int count;

	sigemptyset(&sig_set); //sigset을 다 비워준다
	sigaddset(&sig_set, SIGINT); //sigset에 SIGINT 시그널 추가
	sigprocmask(SIG_BLOCK, &sig_set, NULL); //sigset에 있는 시그널 블럭시켜준다.

	for( count = 3; 0 < count; count--) {
		printf("count %d\n", count);
		sleep(1);
	}

	printf("Ctrl-C에 대한 블록을 해제\n");
	sigprocmask(SIG_UNBLOCK, &sig_set, NULL); //sigset에서 SIGINT 블록 해제시켜준다.
	printf("count 중 Ctrl-C입력하면 이 문장은 출력되지 않음.\n");

	while(1);

	exit(0);
}

