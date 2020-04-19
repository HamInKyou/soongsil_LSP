#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main(void)
{
	char operator;
	FILE *fp;
	int character;
	int number = 0;
	//ssu_expr.txt를 읽기 모드로 open한다.
	if ((fp = fopen("ssu_expr.txt", "r")) == NULL) {
		fprintf(stderr, "fopen error for ssu_expr.txt\n");
		exit(1);
	}
	//fp가 file의 끝에 도달할 때까지 반복
	while (!feof(fp)) {
		//한 문자씩 파일을 읽는데, 파일의 끝에 도달하지 않았거나 숫자일 경우 반복
		while ((character = fgetc(fp)) != EOF && isdigit(character))
			number = 10 * number + character - 48; //number에 숫자로 바꾸어 저장해주기

		fprintf(stdout, " %d\n", number); //바꾸어준 숫자를 출력
		number = 0; //number를 다시 0으로
	
		if (character != EOF) { //파일 끝이 아닐 경우 (위에서 숫자 걸렀으니 숫자 아닌거)
			ungetc(character, fp); //읽은거를 버퍼로 다시 되돌린다.
			operator = fgetc(fp); //버퍼로 되돌린거를 다시 읽어 operator에 저장
			printf("Operator => %c\n", operator); //operator에 저장된거 출력
		}
	}
	fclose(fp); //파일 닫는다
	exit(0);
}
