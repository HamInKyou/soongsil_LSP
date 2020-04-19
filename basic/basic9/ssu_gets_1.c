#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define BUFFER_SIZE 1024

int main(void)
{
	char buf[BUFFER_SIZE];
	//표준입력에서 개행/EOF전까지 읽는다.
	while (fgets(buf, BUFFER_SIZE, stdin) != NULL){
		if (fputs(buf,stdout) == EOF) { //표준출력에 읽은거 출력한다.
			fprintf(stderr, "standard output error\n");
			exit(1);
		}
	}
	if (ferror(stdin)) { //표준입력에 에러가 있다면 에러메시지 출력
		fprintf(stderr, "standard input error\n");
		exit(1);
	}
	exit(0);
}
