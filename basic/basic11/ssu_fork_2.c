#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char *argv[])
{
	pid_t pid;
	char character, first, last;
	long i;

	//부모 프로세스의 경우
	if ((pid = fork()) > 0) {
		first = 'A';
		last = 'Z';
	}
	//자식 프로세스의 경우
	else if (pid == 0) {
		first = 'a';
		last = 'z';
	}
	//fork 에러의 경우
	else {
		fprintf(stderr, "%s\n", argv[0]);
		exit(1);
	}
	
	//부모와 자식 프로세스 경쟁적으로 작업 수행, a-z, A-Z가 섞여서 출력된다.
	for (character = first; character <= last; character++) {
		for (i = 0 ; i <= 100000; i++)
			;

		write(1, &character, 1);
	}

	printf("\n");
	exit(0);
}
