#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	char *fname = "ssu_test.txt";
	char *mode = "r"; 
	if(fopen(fname, mode) == NULL) { //파일을 읽기모드로 오픈한다.
		fprintf(stderr, "fopen error for %s\n", fname);
		exit(1);
	}
	else //성공시 파일 이름과 모드 출력
		printf("Success!\nFilename: <%s>, mode: <%s>\n", fname, mode);

	exit(0);
}
