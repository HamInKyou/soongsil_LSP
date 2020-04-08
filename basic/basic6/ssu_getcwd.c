#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PATH_MAX 1024

int main(void)
{
	char * pathname;

	if(chdir("/home/oslab")<0){ // /home/oslab으로 현재 작업 디렉토리 옮긴다.
		fprintf(stderr, "chdir error\n");
		exit(1);
	}

	pathname = malloc(PATH_MAX); //경로명 담을 pathname 배열 동적할당
	if (getcwd(pathname, PATH_MAX) == NULL) { //현재 작업디렉토리 경로 불러온다.
		fprintf(stderr, "getcwd error\n");
		exit(1);
	}
	printf("current directory = %s\n", pathname); //현재 작업 디렉토리 경로 출력
	exit(0);
}
