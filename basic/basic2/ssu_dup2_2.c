#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

#define BUFFER_SIZE 1024

int main(void)
{
	char buf[BUFFER_SIZE];
	char *fname = "ssu_test.txt";
	int fd;
	int length;

	if((fd = open(fname, O_RDONLY, 0644)) < 0) { //읽기모드로 파일을 오픈
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}

	//표준 출력에 대한 디스크립터를 4번 디스크립터로 복사
	//4번 디스크립터를 통해 쓰기를 했을 경우 표준 출력으로 쓴다.
	if(dup2(1, 4) != 4) {
		fprintf(stderr, "dup2 call failed\n");
		exit(1);
	}

	while(1){
		length = read(fd, buf, BUFFER_SIZE); //파일에서 1024바이트씩 읽는다.

		if(length <= 0) //읽은게 없을경우 종료
			break;

		write(4, buf, length); //4번 디스크립터->표준 출력에다 buf 내용을 쓴다.
	}
}
