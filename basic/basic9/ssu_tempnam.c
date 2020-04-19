#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	char *arg_directory = NULL;
	char *arg_prefix = NULL;

	if (argc != 3) { //명령행에서 인자를 받는다
		fprintf(stderr, "usage: %s <directory> <prefix>\n", argv[0]);
		exit(1);
	}

	arg_directory = argv [1][0] != ' ' ? argv[1]: NULL;
	arg_prefix = argv[2][0] != ' ' ? argv[2]: NULL;
	//첫번째 인자는 임시파일 저장될 디렉토리
	//두번째 인자는 임시파일 앞에 붙을 문장
	printf("created : %s\n", tempnam(arg_directory, arg_prefix));
	exit(0);
}
