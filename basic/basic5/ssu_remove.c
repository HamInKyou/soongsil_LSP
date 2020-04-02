#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(int argc, char *argv[])
{
	if (argc != 3) {
		fprintf(stderr, "usage: %s <oldname> <newname>\n", argv[0]);
		exit(1);
	}

	//하드링크 해준다.
	if (link(argv[1], argv[2]) < 0) {
		fprintf(stderr, "link error\n");
		exit(1);
	}
	else
		printf("step1 passed.\n");
	
	//원본파일 연결링크 카운터 감소시킨다.
	//오픈되어있는 상태가 아니기 때문에 삭제된다.
	if(remove(argv[1]) < 0) {
		fprintf(stderr, "remove error\n");
		remove(argv[2]);
		exit(1);
	}
	else
		printf("step2 passed.\n");
	
	printf("Success!\n");
	exit(0);
}
