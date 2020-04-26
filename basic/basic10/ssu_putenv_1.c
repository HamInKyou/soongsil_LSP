#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void ssu_addone(void);

extern char **environ; //환경 변수들이 문자열형태로 저장된 이차원 배열
                       //전역 변수로 선언할 필요성이 있다.
char glob_var[] = "HOBBY=swimming";

int main(void)
{
	int i;
	//현재 환경 변수들 다 출력
	for ( i = 0; environ[i] != NULL; i++)
		printf("environ[%d] : %s\n", i, environ[i]);

	putenv(glob_var); //새로운 환경변수를 등록한다.
	ssu_addone(); //지역변수를 인자로 사용하여 새로운 환경 변수를 등록
	//기존 환경변수 리스트 중에서 인자로 준 이름에 따른 값에 대한 포인터를 리턴하여 출력
	printf("My hobby is %s\n", getenv("HOBBY"));
	printf("My lover is %s\n", getenv("LOVER"));
	strcpy(glob_var + 6, "fishing"); //swimming을 fishing으로 변경
	
	//현재 환경 변수들 다 출력
	for( i = 0; environ[i] != NULL; i++ )
		printf("environ[%d] : %s\n", i, environ[i]);

	exit(0);
}

void ssu_addone(void) {
	char auto_var[10]; //지역변수를 putenv()의 인자로 사용
	strcpy(auto_var, "LOVER=js");
	putenv(auto_var);
}

