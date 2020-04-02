#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

int main(int argc, char *argv[])
{
	int fd;
	if (argc != 3) {
		fprintf(stderr, "usage : %s <oldname> <newname>\n", argv[0]);
		exit(1);
	}
	// 읽기모드로 파일 열기
	if ((fd = open(argv[1], O_RDONLY)) < 0){
		fprintf(stderr, "first open error for %s\n", argv[1]);
		exit(1);
	}
	else //파일 열기 성공했을경우 바로 닫아주기
		close(fd);
	
	//파일 이름 바꾸기
	if (rename(argv[1], argv[2]) < 0) {
		fprintf(stderr, "rename eorror\n");
		exit(1);
	}

	//파일 바꾸기 전 이름으로 파일 열기 -> 이름 바꾼 상태라 못찾음
	if ((fd = open(argv[1], O_RDONLY)) < 0)
		printf("second open error for %s\n", argv[1]);
	else {
		fprintf(stderr, "it's very strange!\n");
		exit(1);
	}

	//바꾼 이름으로 파일 열기
	if ((fd = open(argv[2], O_RDONLY)) < 0){
		fprintf(stderr, "third open error for %s\n", argv[2]);
		exit(1);
	}

	printf("Everything is good!\n");
	exit(0);
}
