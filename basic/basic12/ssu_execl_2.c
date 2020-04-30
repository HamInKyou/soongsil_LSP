#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	//자식프로세스 생성
	if (fork() == 0) {
		// /bin/echo를 exec하는데 인자들을 넘겨줌
		execl("/bin/echo", "echo", "this is", "message one", (char *)0);
		fprintf(stderr, "exec error\n");
		exit(1);
	}
	//자식프로세스 생성
	if (fork() == 0) {
		// /bin/echo를 exec하는데 인자들을 넘겨줌
		execl("/bin/echo", "echo", "this is", "message two", (char *)0);
		fprintf(stderr, "exec error\n");
		exit(1);
	}
	//자식프로세스 생성
	if (fork() == 0) {
		// /bin/echo를 exec하는데 인자들을 넘겨줌
		execl("/bin/echo", "echo", "this is", "message three", (char *)0);
		fprintf(stderr, "exec error\n");
		exit(1);
	}
	printf("Parent program ending\n");
	exit(0); //부모는 끝나도 자식 프로세스는 별개로 돌아감
}
