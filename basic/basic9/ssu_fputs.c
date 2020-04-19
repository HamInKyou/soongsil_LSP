#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int main (int argc, char *argv[])
{
	char buf[BUFFER_SIZE];
	FILE *fp;
	if(argc != 2) {
		fprintf(stderr, "usage : %s <file>\n", argv[0]);
		exit(1);
	}
	//파일 읽고쓰기 모드로 오픈
	if((fp = fopen(argv[1], "w+")) == NULL) {
		fprintf(stderr, "fopen error for %s\n", argv[1]);
		exit(1);
	}
	fputs("Input String >> ", stdout);
	gets(buf); //표준입력에서 받아서 buf에 저장
	fputs(buf, fp); //buf에 있는거 표준 출력으로 출력
	rewind(fp); //fp 위치 맨 첨 위치로
	fgets(buf, sizeof(buf), fp);
	puts(buf); //buf에 있는거 표준 출력으로 출력
	fclose(fp); //파일 닫는다.
	exit(0);
}
