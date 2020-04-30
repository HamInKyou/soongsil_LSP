#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(void)
{
	int fd, state;
	
	//유효사용자 아이디를 1000으로 바꾼다.
	state = seteuid(1000);
	
	if (state < 0) { //seteuid 에러시
		fprintf(stderr, "seteuid error\n");
		exit(1);
	}
	
	//ssu_test.txt를 읽고쓰기 모드로 생성하고, 연다. 유저에게 읽기쓰기실행 권한을 준다.
	if ((fd = open("ssu_test.txt", O_CREAT|O_RDWR, S_IRWXU)) < 0) {
		fprintf(stderr, "open error\n");
		exit(1);
	}
	
	//파일을 닫는다.
	close(fd);
	exit(0);
}
