#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int i;
	//명령행 인자로 들어온 값들을 차례대로 출력해줌
	for (i = 0; i < argc; i++)
		printf("argv[%d] : %s\n", i, argv[i]);
	exit(0);
}
