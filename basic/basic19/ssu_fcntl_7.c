#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(void)
{
	char *filename = "ssu_test.txt";
	int fd1, fd2;
	int flag;
	
	//파일을 읽고쓰기모드, O_APPEND 속성을 키고 오픈한다.
	if ((fd1 = open(filename, O_RDWR | O_APPEND, 0644)) < 0) {
		fprintf(stderr, "open error for %s\n", filename);
		exit(1);
	}
	
	//CLOEXEC 속성을 파일에 적용시켜준다.
	if (fcntl(fd1, F_SETFD, FD_CLOEXEC) == -1) {
		fprintf(stderr, "fcntl F_SETFD error\n");
		exit(1);
	}
	//파일의 속성을 읽어온다.
	if ((flag = fcntl(fd1, F_GETFL, 0)) == -1) {
		fprintf(stderr, "fcntl F_GETFL error\n");
		exit(1);
	}
	
	//O_APPEND 속성이 켜져있다면
	if (flag & O_APPEND)
		printf("fd1 : O_APPEND flag is set.\n");
	else
		printf("fd1 : O_APPEND flag is NOT set.\n");

	//파일의 속성을 읽어온다.
	if ((flag = fcntl(fd1, F_GETFD, 0)) == -1) {
		fprintf(stderr, "fcntl F_GETFD error\n");
		exit(1);
	}
	
	//FD_CLOEXEC 속성이 켜져있다면
	if (flag & FD_CLOEXEC)
		printf("fd1 : FD_CLOEXEC flag is set.\n");
	else
		printf("fd1 : FD_CLOEXEC flag is NOT set.\n");
	
	//fd2는 fd1의 파일 디스크립터를 복사했다.
	if ((fd2 = fcntl(fd1, F_DUPFD, 0)) == -1) {
		fprintf(stderr, "fcntl F_DUPFD error\n");
		exit(1);
	}
	
	//fd2의 파일 속성을 읽어온다.
	if ((flag = fcntl(fd2, F_GETFL, 0)) == -1) {
		fprintf(stderr, "fcntl F_GETFL error\n");
		exit(1);
	}
	
	//O_APPEND 속성이 켜져있다면
	if (flag & O_APPEND)
		printf("fd2 : O_APPEND flag is set.\n");
	else
		printf("fd2 : O_APPEND flag is NOT set.\n");
	
	//fd2의 파일 속성을 읽어온다.
	if ((flag = fcntl(fd2, F_GETFD, 0)) == -1) {
		fprintf(stderr, "fcntl F_GETFD error\n");
		exit(1);
	}
	
	//FD_CLOEXEC이 켜져있는가? -> 파일 디스크립터 복사시
	//                            close-on-exec 기능은 복사되지 않는다.
	if (flag & FD_CLOEXEC)
		printf("fd2 : FD_CLOEXEC flag is set.\n");
	else
		printf("fd2 : FD_CLOEXEC flag is NOT set.\n");

	exit(0);
}
