#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(int argc, char *argv[])
{
	int i;

	if (argc < 2) {
		fprintf(stderr, "usage: %s <file1> <file2> .. <fileN>\n", argv[0]);
		exit(1);
	}

	for(i = 1; i < argc; i++) {
		if(access(argv[i], F_OK) < 0) { //파일이 존재하는가? 성공시 0 리턴
			fprintf(stderr, "%s dosen't exist.\n", argv[i]);
			continue;
		}

		if(access(argv[i], R_OK) == 0) //읽기 가능한가? 성공시 0 리턴
			printf("User can read %s\n", argv[i]);

		if(access(argv[i], W_OK) == 0) //쓰기 가능한가? 성공시 0 리턴
			printf("User can write %s\n", argv[i]);

		if(access(argv[i], X_OK) == 0) //실행 가능한가? 성공시 0 리턴
			printf("User can execute %s\n", argv[i]);
	}

	exit(0);
}
