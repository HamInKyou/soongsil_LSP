#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void ssu_charatatime(char *str);

int main(void)
{
	pid_t pid;
	
	//fork 에러의 경우
	if ((pid = fork()) < 0) {
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	//자식 프로세스일 경우
	else if (pid == 0)
		ssu_charatatime("output from chile\n");
	//부모 프로세스일 경우
	else
		ssu_charatatime("output from parent\n");
	
	exit(0);
}

//인자로 받은 문자열 버퍼에 담지않고 바로바로 출력
//부모 프로세스와 자식 프로세스가 경쟁적으로 한글자씩 출력한다.
static void ssu_charatatime(char *str) {
	char *ptr;
	int print_char;

	setbuf(stdout, NULL); //버퍼 안들리고 즉시 나오게

	for (ptr = str; (print_char = *ptr++) != 0; ) {
		putc(print_char, stdout);
		usleep(10); //출력하고 10마이크로초 대기
	}
}
