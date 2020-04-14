#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int character;
	while ((character = getc(stdin)) != EOF) //표준입력에서 EOF 도달할 때까지 한문자씩 받는다.
	{                                        //표준입력에서 EOF는 ctrl+D 입력했을 경우
		if (putc(character, stdout) == EOF) { //받은 문자열을 터미널에 출력한다.
			fprintf(stderr, "standard output error\n");
			exit(1);
		}
	}
	if (ferror(stdin)) { //주어진 파일 스트림의 읽기 또는 쓰기 시 에러를 검사
		fprintf(stderr, "standard input error\n");
		exit(1);
	}
	exit(0);
}
