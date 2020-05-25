#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

int main(void)
{
	int flag;
	
	//표준 출력의 파일 디스크립터 복사본 만든다.
	//인자 하나 부족하므로 에러 처리
	if ((flag = fcntl(STDOUT_FILENO, F_DUPFD)) == -1) {
		fprintf(stderr, "Error : Checking CLOSE_ON_EXEC\n");
		exit(1);
	}

	printf("CLOSE ON EXEC flag is = %d\n", flag);
	exit(0);
}
