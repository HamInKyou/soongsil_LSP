#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	char *fname = "ssu_test.txt";
	FILE *fp;

	if((fp = fopen(fname, "r")) == NULL) { //파일을 읽기모드로 연다.
		fprintf(stderr, "fopen error for %s\n", fname);
		exit(1);
	}
	else{ //성공했을시
		printf("Success!\n"); 
		printf("Opening \"%s\"in\"r\" mode!\n", fname); //파일명과 모드를 출력한다.
	}

	fclose(fp);
	exit(0);
}


