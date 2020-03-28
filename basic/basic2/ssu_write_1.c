#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define BUFFER_SIZE 1024

int main(void)
{
	char buf[BUFFER_SIZE];
	int length;

	length = read(0, buf, BUFFER_SIZE); //표준 입력의 내용을 버퍼사이즈만큼 읽는다.
	write(1, buf, length);//표준 출력에 버퍼의 내용을 길이만큼 쓴다.
	exit(0);
}
