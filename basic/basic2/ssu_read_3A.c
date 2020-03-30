#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

int main(void)
{
	char c; //문자하나 담을 변수
	int fd;
	if((fd = open("ssu_test.txt",O_RDONLY))<0){ //읽기모드로 파일 오픈
		fprintf(stderr, "open error for %s\n", "ssu_test.txt");
		exit(1);
	}

	while(1) { //한글자씩 읽어서 화면으로 출력
		if(read(fd,&c, 1) > 0){
			putchar(c);
		}
		else
			break;
	}
	exit(0);
}
