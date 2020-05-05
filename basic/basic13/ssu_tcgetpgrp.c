#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <termios.h>

#define STDIN_NUMBER 0

static void ssu_sig_hup(int signum);
static void ssu_print_ids(char *name);

int main(void)
{
	pid_t pid;
	char character;
	
	ssu_print_ids("parent"); //부모와 관련된 id 출력하기
	//자식 프로세스 생성
	if ((pid = fork()) < 0) { //에러인 경우
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	//부모인 경우
	else if (pid > 0) {
		sleep(3);
		exit(0);
	}
	//자식의 경우
	else {
		ssu_print_ids("child"); //자식과 관련된 id 출력
		signal(SIGHUP, ssu_sig_hup); //SIGHUB 시그널 받으면 ssu_sig_hup 실행
		kill(getpid(), SIGTSTP); //현 프로세스에 SIGTSTP 시그널 보냄
		ssu_print_ids("child");
		
		//표준입력에서 한글자 읽는다.
		if (read(STDIN_FILENO, &character, 1) != 1)
			fprintf(stderr, "read error\n");

		exit(0);
	}
}

static void ssu_sig_hup(int signum) {
	printf("ssu_SIG_HUP received, pid = %d\n", getpid());
}

//현 프로세스의 id, 프로세스의 부모 id, 그룹 id, foreground process group id 출력
static void ssu_print_ids(char *name) {
	printf("[%s] : pid = %d, ppid = %d, pgrp = %d, tpgrp = %d\n", 
			name, getpid(), getppid(), getpgrp(), tcgetpgrp(STDIN_NUMBER));
	fflush(stdout); //표준출력 버퍼 비운다.(출력)
}
