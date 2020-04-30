#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	extern char **environ;
	char **str;
	int i;

	for (i = 0; i < argc; i++) //명령행 인자로 받은것들 출력
		printf("argv[%d]: %s\n", i, argv[i]);
	
	for (str = environ; *str != 0; str++) //환경변수들 출력
		printf("%s\n", *str);
	
	exit(0);
}
