#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(int argc, char *argv[]) {
	if (argc != 3) {
		fprintf(stderr, "usage : %s <acutalname> <symname>\n", argv[0]);
		exit(1);
	}
	//심볼릭 링크 생성, argv[1]에 해당하는 파일을 가리키는 심볼릭파일 생성
	if (symlink(argv[1], argv[2]) < 0) {
		fprintf(stderr, "symlink error\n");
		exit(1);
	}
	else
		printf("symlink: %s -> %s\n", argv[2], argv[1]);
	exit(0);
}
