#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	char *fname = "ssu_test.txt";
	FILE *fp;
	long position;
	int character;
	
	//ssu_test.txt 읽기 모드로 오픈
	if ((fp = fopen(fname, "r")) == NULL) {
		fprintf(stderr, "fopen error for %s\n", fname);
		exit(1);
	}

	printf("Input number >>");
	scanf("%ld", &position); //long타입 정수 입력받아 position에 저장
	fseek(fp, position - 1, SEEK_SET); //fp의 위치를 position위치로
                                      //-1은 offset은 0부터 시작하기에 맞추기 위해
	character = getc(fp); //파일에서 글자 하나 읽는다.
	printf("%ldth character => %c\n", position, character);
	exit(0);
}
