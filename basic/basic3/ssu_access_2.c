#include<stdio.h>
#include<stdlib.h>
#include<unistd.h> //access()쓰기 위해 필요한 헤더

//table구조체 배열의 사이즈에서 table구조체 배열 한칸의 사이즈를 나누면
//table구조체 배열이 몇칸인지 나온다.
#define TABLE_SIZE (sizeof(table)/sizeof(*table))

int main(int argc, char *argv[])
{
	struct {
		char *text;
		int mode;
	} table[] = {
		{"exists", 0},
		{"execute", 1},
		{"write", 2},
		{"read", 4}
	}; //파일의 권한 정보를 담은 구조체 선언과 생성

	int i;

	//실행 입력이 적절히 잘 이루어졌는지
	if (argc < 2) {
		fprintf(stderr, "usage : %s <file>\n", argv[0]);
		exit(1);
	}

	//파일에 대한 접근 권한을 하나씩 검사하며 가능한지 불가능한지 출력
	for (i = 0; i < TABLE_SIZE; i++) {
		if(access(argv[1], table[i].mode) != -1)
			printf("%s -ok\n", table[i].text);
		else
			printf("%s\n", table[i].text);
	}
	exit(0);
}
