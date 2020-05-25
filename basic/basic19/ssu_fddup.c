#include<fcntl.h>
#include<sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

int main(void)
{
	int testfd;
	int fd;

	fd = open("test.txt", O_CREAT); //test.txt를 오픈

	testfd = fcntl(fd, F_DUPFD, 5); //파일 디스크립터 5로 복사
	printf("testfd : %d\n", testfd);
	testfd = fcntl(fd, F_DUPFD, 5); //파일 디스크립터 5로 복사
	                                //5 이미 사용중, 6으로 복사
	printf("testfd : %d\n", testfd);

	getchar();
}
