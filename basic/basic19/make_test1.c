#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void){
	FILE *fp;
	char *fname = "ssu_test1.txt";
	int i, num;

	fp = fopen(fname, "w");
	srand((unsigned int)time(NULL));
	for(i = 0; i < 500000; i++)
	{
		num = rand() % 10;
		fprintf(fp, "%d", num);
	}
	fclose(fp);
}
