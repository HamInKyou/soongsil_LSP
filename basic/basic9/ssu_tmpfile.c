#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int main(void)
{
	char buf[BUFFER_SIZE];
	char name[L_tmpnam];
	FILE *fp;

	//임시파일 만들어서 경로 출력
	printf("Temporary filename <<%s>>\n", tmpnam(name));
	
	//임시파일 만들고 fp가 그 파일 가르키게
	if ((fp = tmpfile()) == NULL) {
		fprintf(stderr, "tmpfile create error!!\n");
		exit(1);
	}
	//임시파일에 출력한다.
	fputs("create tmpfile success!!\n", fp);
	rewind(fp); //fp를 파일 맨 앞으로 돌린다.
	fgets(buf, sizeof(buf), fp); //파일에서 버퍼사이즈만큼 읽는다.
	puts(buf); //버퍼에 저장된 내용 출력
	exit(0);
}
