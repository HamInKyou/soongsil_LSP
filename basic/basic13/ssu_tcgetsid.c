#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <termios.h>

int main(void)
{
	pid_t sid_stderr;
	pid_t sid_stdin;
	pid_t sid_stdout;

	//stdin에 해당하는 제어 터미널과 연관된 세션리더 프로세스 그룹 ID 리턴
	sid_stdin = tcgetsid(STDIN_FILENO);
	
	if(sid_stdin == -1) {
		fprintf(stderr, "tcgetsid error\n");
		exit(1);
	}
	else //표준 입력에 대한 제어 터미널의 세션 ID 출력
		printf("Session Leader for stdin: %d\n", sid_stdin);

	//stdout에 해당하는 제어 터미널과 연관된 세션리더 프로세스 그룹 ID 리턴
	sid_stdout = tcgetsid(STDOUT_FILENO);

	if(sid_stdout == -1) {
		fprintf(stderr, "tcgetsid error\n");
		exit(1);
	}
	else //표준 출력에 대한 제어 터미널의 세션 ID 출력
		printf("Session Leader for stdout: %d\n", sid_stdout);

	//stderr에 해당하는 제어 터미널과 연관된 세션리더 프로세스 그룹 ID 리턴
	sid_stderr = tcgetsid(STDERR_FILENO);

	if(sid_stderr == -1) {
		fprintf(stderr, "tcgetsid error\n");
		exit(1);
	}
	else //표준 에러에 대한 제어 터미널의 세션 ID 출력
		printf("Session Leader for stderr: %d\n", sid_stderr);

	exit(0);
}

