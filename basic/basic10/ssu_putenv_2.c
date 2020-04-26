#include <stdio.h>
#include <stdlib.h>

void ssu_printenv(char *label, char ***envpp);

extern char **environ;

//인자로 환경변수도 받아온다.
int main(int argc, char *argv[], char *envp[])
{	
	ssu_printenv("Initially", &envp); 
	putenv("TZ=PST8PDT"); //새로운 환경 변수를 추가한다.
	ssu_printenv("After changing TZ", &envp);
	putenv("WARNNING=Don't use envp after putenv()"); //새로운 환경변수 추가
	ssu_printenv("After setting a new variable", &envp);
	printf("value of WARNNING is %s\n", getenv("WARNING"));
	exit(0);
}

//현재 환경 변수들에 대한 위치, 값 정보들을 출력하는 함수
void ssu_printenv(char *label, char ***envpp) {
	char **ptr;

	printf("----%s---\n", label);
	printf("envp is at %8o and contains %8o\n", envpp, *envpp);
	printf("environ is at %8o and contains %8o\n", &environ, environ);
	printf("My environment variable are:\n");
	for(ptr = environ; *ptr; ptr++)
		printf("(%8o) = %8o -> %s\n", ptr, *ptr, *ptr);
	printf("(%8o) = %8o\n", ptr, *ptr);
}
