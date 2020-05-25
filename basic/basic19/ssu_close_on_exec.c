#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(void)
{
	int fd;
	int val;
	
	//exec_copy.txt를 생성하고, 연다.
	if ((fd = open("exec_copy.txt", O_CREAT)) < 0) {
		fprintf(stderr, "open error for %s\n", "exec_copy.txt");
		exit(1);
	}
	//파일의 속성을 불러와 저장한다.
	val = fcntl(fd, F_GETFD, 0);

	//CLOEXEC 속성이 켜져있으면
	if (val & FD_CLOEXEC)
		printf("close-on-exec bit on\n");
	else
		printf("close-on-exec bit off\n");
	
	//CLOEXEC 속성을 켜준다.
	val |= FD_CLOEXEC;
	
	//CLOEXEC 속성이 켜져 있으면
	if (val & FD_CLOEXEC)
		printf("close-on-exec bit on\n");
	else
		printf("close-on-exec bit off\n");

	//CLOEXEC 속성을 켜주고 파일에 적용시켜준다.
	//exec하고 실행할 때 오픈한 파일을 상속해주지 않겠다.
	fcntl(fd, F_SETFD, val);
	//loop를 실행시켜준다.
	execl("/home/ingyu/LSP/basic/basic19/loop", "./loop", NULL);
}
