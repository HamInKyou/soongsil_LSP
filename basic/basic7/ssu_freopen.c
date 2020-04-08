#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	char *fname = "ssu_test.txt";
	int fd;

	printf("First printf : Hello, OSLAB!!\n");

	if((fd = open(fname ,O_RDONLY)) < 0) { //파일을 읽기모드로 오픈
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}

	if(freopen(fname, "w", stdout) != NULL) //표준 출력을 fname으로 바꾸어준다.
		printf("Second printf : Hello, OSLAB!!\n"); //이 내용은 fname에 해당하는 파일에 찍힌다.

	exit(0);
}
