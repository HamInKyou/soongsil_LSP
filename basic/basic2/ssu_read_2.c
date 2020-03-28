#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //read() 쓰기 위해 선언한 헤더
#include <fcntl.h> //open() 쓰기 위해 선언한 헤더

#define BUFFER_SIZE 1024 //버퍼의 사이즈 상수선언

int main(void)
{
	char buf[BUFFER_SIZE]; //버퍼 생성
	char *fname = "ssu_test.txt";
	int count;
	int fd1, fd2; //파일 디스크립터 두개 선언

	//똑같은 파일을 두번 연다
	fd1 = open(fname, O_RDONLY, 0644);
	fd2 = open(fname, O_RDONLY, 0644);

	//오픈이 잘못 됐을 경우 에러표시하고 끝내기
	if (fd1 < 0 || fd2 < 0) {
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}
	
	count = read(fd1, buf, 25); //파일을 25바이트만큼 읽는데, 읽은만큼의 바이트를 리턴
	buf[count] = 0; //버퍼에 넣은 문자의 끝에 null문자를 넣어 끝을 표시
	printf("fd1's first printf : %s\n", buf);
	lseek(fd1, 1, SEEK_CUR);//개행문자를 뛰어넘기 위해 한칸 뒤로 파일 디스크립터를 민다.
	count = read(fd1, buf, 24);
	buf[count] = 0;
	printf("fd1's second printf : %s\n", buf);
	count = read(fd2, buf, 25);
	buf[count] = 0;

	//fd2는 fd1과 별개로 새로 파일을 연 것이기 때문에 그 위치는 여전히 처음 위치이다.
	printf("fd2's first printf : %s\n", buf);
	lseek(fd2, 1, SEEK_CUR);
	count = read(fd2, buf, 24);
	buf[count] = 0;
	printf("fd2's second printf : %s\n", buf);
	
	exit(0);
}
