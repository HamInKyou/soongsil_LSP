#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	//인자로 넘겨줄 명령행 인자들
	char *argv[] = {
		"ssu_execl_test_1", "param1", "param2", (char *)0
	};
	//인자로 넘겨줄 환경변수들
	char *env[] = {
		"NAME = value",
		"nextname=nextvalue",
		"HOME=/home/oslab",
		(char *)0
	};
	
	printf("this is the original program\n");
	// ./ssu_execl_test_1 exec하는데 argv와 env 인자로 넘겨준다.
	execve("./ssu_execl_test_1", argv, env);
	//execve로 인하여 프로세스 넘어갔으므로 아래 프린트문 출력되지 않는다.
	printf("%s\n", "This line should never get printed\n");
	exit(0);
}
