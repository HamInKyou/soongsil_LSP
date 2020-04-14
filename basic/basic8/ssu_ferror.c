#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	FILE *fp;
	int character;

	if(argc != 2) {
		fprintf(stderr, "usage: %s <filename>\n", argv[0]);
		exit(1);
	}
	//인자로 들어온 파일 읽기모드로 오픈
	if ((fp = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "fopen error for %s\n", argv[1]);
		exit(1);
	}
	//파일에서 한글자를 읽는다.
	character = fgetc(fp);
	while (!feof(fp)) { //함수의 끝에 도달할 때까지 반복
		fputc(character, stdout); //표준 출력으로 출력한다.
		if (ferror(fp)) { //에러 검사
			fprintf(stderr, "Error detected!!\n");
			clearerr(fp); //파일 스트림의 에러 플래그를 지워준다.
		}
		character = fgetc(fp); //파일에서 한글자를 읽는다.
	}
	fclose(fp); //파일을 닫는다.
	exit(0);
}
