#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

#define BUFFER_SIZE 1024

int main(void)
{
	char buf[BUFFER_SIZE];
	char *fname = "ssu_test.txt";
	int count;
	int fd1, fd2;
	
	//파일을 읽기모드로 연다.
	if ((fd1 = open(fname,O_RDONLY,0644)) < 0){
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}
	
	//fd1 디스크립터를 복사한 디스크립터를 fd2에 배정
	fd2 = dup(fd1);
	count = read(fd1, buf, 12); //fd1 디스크립터로 12바이트만큼 파일에서 읽는다.
	buf[count] = 0;//읽은 것의 끝에 널문자를 넣는다.
	printf("fd1's printf: %s\n", buf);
	lseek(fd1, 1, SEEK_CUR); //띄어쓰기 뛰어넘기 위해

	//fd1 디스크립터를 복사하여 fd1의 내용과 똑같은 fd2 디스크립터로 12바이트만큼 읽는다.
	//fd1에 대한 오프셋 위치가 옮겨져있는 상태이기 때문에 fd2도 이동한 그 위치에서 시작한다.
	count = read(fd2, buf, 12);
	buf[count] = 0;
	printf("fd2's printf: %s\n", buf);
	exit(0);
}
