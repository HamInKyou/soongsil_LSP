#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	pid_t pgid;
	pid_t pid;

	if (argc < 2) { 
		fprintf(stderr, "usage: %s <file>\n", argv[0]);
		exit(1);
	}

	pid = getpid(); //현재 프로세스 id를 검색한다.
	pgid = getpgid(atoi(argv[1])); //명령행 첫번째 인자로 받은 pid의 그룹 pid를 검색한다.
	printf("pid: %d, pgid: %d\n", pid, pgid);
	exit(0);
}
