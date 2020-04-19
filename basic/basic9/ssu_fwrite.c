#include <stdio.h>
#include <stdlib.h>

struct ssu_id {
	char name[64];
	int id;
};

int main(void)
{
	struct ssu_id test1, test2;
	char *fname = "ssu_exam.dat";
	FILE *fp;
	
	//파일을 쓰기모드로 오픈
	if ((fp = fopen(fname, "w")) == NULL) {
		fprintf(stderr, "fopen error for %s\n", fname);
		exit(1);
	}
	
	//ID와 name 입력해서 구조체에 넣는다.
	printf("Input ID>> ");
	scanf("%d", &test1.id);
	printf("Input name>> ");
	scanf("%s", test1.name);

	//구조체 정보를 파일에 쓴다.
	if (fwrite(&test1, sizeof(struct ssu_id), 1, fp) != 1) {
		fprintf(stderr, "fwrite error\n");
		exit(1);
	}
	
	fclose(fp); //파일 닫는다.

	//파일을 읽기모드로 오픈
	if ((fp = fopen(fname, "r")) == NULL) {
		fprintf(stderr, "fopen error for %s\n", fname);
		exit(1);
	}

	//구조체 정보를 파일에서 읽어서 test2에 저장한다.
	if (fread(&test2, sizeof(struct ssu_id), 1, fp) != 1) {
		fprintf(stderr, "fread error\n");
		exit(1);
	}

	printf("\nID    name\n");
	printf(" ============\n");
	printf("%d      %s\n", test2.id, test2.name);
	fclose(fp);
	exit(0);
}
