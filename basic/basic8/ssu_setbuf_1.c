#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(void)
{
	char buf[BUFFER_SIZE];
	setbuf(stdout, buf); //stdout에서 데이터 입출력 발생할 경우
							   //버퍼를 buf로 설정
	printf("Hello, ");
	sleep(1);
	printf("OSLAB!!");
	sleep(1);
	printf("\n");
	sleep(1);
	setbuf(stdout, NULL); //버퍼를 사용하지 않고 바로 입출력
	printf("How");        //위에 버퍼에 담겨있던 것들과 함께 한꺼번에 출력
	sleep(1);
	printf(" are");
	sleep(1);
	printf(" you?");
	sleep(1);
	printf("\n");
	exit(0);
}
