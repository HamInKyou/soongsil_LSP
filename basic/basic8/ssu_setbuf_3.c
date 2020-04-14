#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int main(void)
{
	char buf[BUFFER_SIZE];
	int a, b;
	int i;
	setbuf(stdin, buf); //stdin에서 입출력 발생할 경우 buf로 임시저장
	scanf("%d %d", &a, &b); //scanf한거 buf에 쌓여있음
	for(i = 0; buf[i] != '\n'; i++) //개행이 나올 때까지 반복
		putchar(buf[i]); //버퍼에 담긴거를 한글자씩 출력해준다.

	putchar('\n'); //개행 출력을 제외했으므로, 추가로 개행을 출력해준다.
	exit(0);
}
