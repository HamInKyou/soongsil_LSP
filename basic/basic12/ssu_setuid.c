#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	char line[256];
	FILE *fp;
	int uid;

	if (argc < 2) { //인자 제대로 받았는지
		fprintf(stderr, "usage: %s file_name\n", argv[0]);
		exit(1);
	}

	//실제 사용자 ID와 사용자 유효 ID를 출력한다.
	printf("initially uid = %d and euid = %d\n", getuid(), geteuid());
	fp = fopen(argv[1], "r"); //파일을 읽기모드로 오픈한다.

	if (fp == NULL) { //오픈이 실패했을 경우
		fprintf(stderr, "first open error for %s\n", argv[1]);
		exit(1);
	}
	else {
		printf("first open successful:\n");

		while (fgets(line, 255, fp) != NULL) //파일을 한줄 읽어서
			fputs(line, stdout); //읽은거 출력

		fclose(fp);
	}

	//실제 사용자 ID 받아와서 유효 사용자 ID를 프로그램 파일 소유자 ID로 바꿔준다.
	setuid(uid = getuid());
	printf("after setuid(%d):\n uid=%d and euid=%d\n", uid, getuid(), geteuid());
	fp = fopen(argv[1], "r"); //파일을 읽기모드로 오픈한다.

	if (fp == NULL) { //오픈에 실패했을 경우
		fprintf(stderr, "second open error for %s\n", argv[1]);
		exit(1);
	}
	else {
		printf("second open successful:\n");
		
		while (fgets(line, 255, fp) != NULL) //파일을 한줄 읽어서
			fputs(line, stdout); //읽은거 출력

		fclose(fp);
	}
	exit(0);
}
