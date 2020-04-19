#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE 4096

int main(void)
{
	char buf[MAX_LINE];
	char name[L_tmpnam]; //임시파일 이름을 저장할 배열
	FILE *fp;
	printf("temp file 1 : %s\n", tmpnam(NULL)); //임시파일을 생성
	tmpnam(name); //임시파일을 생성하여 name에 그 경로 저장
	printf("temp file 2 : %s\n", name); //name에 저장한 임시파일의 경로 출력
	if ((fp = tmpfile()) == NULL) { //임시파일 생성
		fprintf(stderr, "tmpfile error\n");
		exit(1);
	}
	fputs("tmpfile created temporary file.\n", fp);
	fseek(fp, 0, SEEK_SET); //fp의 위치를 파일 맨 처음으로
	if (fgets(buf, sizeof(buf), fp) == NULL) { //버퍼 사이즈만큼 파일에서 읽는다.
		fprintf(stderr, "fgets error\n");
		exit(1);
	}
	fputs(buf, stdout); //버퍼에 저장된 내용을 표준출력으로 출력한다.
	exit(0);
}
