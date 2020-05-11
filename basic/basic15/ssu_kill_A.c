#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char *argv[])
{
	if (argc != 2) { //명령행 두번째 인자(pid) 받아서
		fprintf(stderr, "usage: %s [Process ID]\n", argv[0]);
		exit(1);
	}
	else //SIGKILL 시그널 위에서 받은 pid로 보낸다.
		kill(atoi(argv[1]), SIGKILL);
}
