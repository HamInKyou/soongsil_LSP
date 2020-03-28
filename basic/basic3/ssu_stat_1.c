#include<stdio.h>
#include<stdlib.h>
#include<unistd.h> //stat()에 필요한 헤더
#include<sys/types.h> //stat()에 필요한 헤더
#include<sys/stat.h> //stat()에 필요한 헤더

int main(int argc, char *argv[])
{
	struct stat statbuf;//파일의 정보를 갖는 stat구조체를 담을 변수

	//제대로 입력했는지 에러판단
	if (argc != 2) {
		fprintf (stderr, "usage : %s <file>\n", argv[0]);
		exit(1);
	}

	//statbuf에 argv[1]의 정보를 담은 stat구조체를 리턴한다.
	if((stat(argv[1],&statbuf)) < 0 ) {
		fprintf(stderr, "stat error\n");
		exit(1);
	}
	
	//st_size는 파일의 크기 정보를 담고있다.
	printf("%s is %ld bytes\n", argv[1], statbuf.st_size);
	exit(0);
}
