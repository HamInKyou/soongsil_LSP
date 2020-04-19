#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int main(void)
{
	char *fname = "ssu_test.dat";
	char name[BUFFER_SIZE];
	FILE *fp;
	int age;
	
	//ssu_test.dat을 읽기모드로 오픈
	fp = fopen(fname, "r");
	fscanf(fp, "%s%d", name, &age); //파일에서 읽는다.
	fclose(fp); //파일 닫는다.
	fp = fopen(fname, "w"); //파일을 쓰기모드로 오픈
	fprintf(fp, "%s is %d years old\n", name, age);
	fclose(fp);
	exit(0);
}
