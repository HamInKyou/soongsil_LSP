#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

int main(void)
{
	char *fname = "ssu_test.txt";
	int fd;

	if((fd = creat(fname, 0666)) < 0){ //파일을 생성한다. 이미 있을 경우 내용 다 지움
		printf("creat error for %s\n", fname);
		exit(1);
	}

	printf("First printf is on the screen.\n");
	dup2(fd, 1); 
	//파일 디스크립트 fd의 내용을 표준 출력 디스크립트에 복사
	//->표준 출력에서 나올 내용은 앞으로 fd가 가리키는 파일에 출력된다.
	printf("Second printf is in this file.\n");
	exit(0);
}
