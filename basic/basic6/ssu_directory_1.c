#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#define DIRECTORY_SIZE MAXNAMLEN

int main(int argc, char *argv[]) {
	struct dirent *dentry; //DIR 구조체 담기 위해
	struct stat statbuf;
	char filename[DIRECTORY_SIZE + 1]; //파일이름 담는 배열
	DIR *dirp; //DIR 가리키는 포인터

	if (argc < 2) {
		fprintf(stderr, "usage: %s <directory>\n", argv[0]);
		exit(1);
	}

	//해당 디렉토리 열고 현재작업 디렉토리를 해당 디렉토리로 변경한다.
	if ((dirp = opendir(argv[1])) ==NULL || chdir(argv[1])==-1) { 
		fprintf(stderr, "opendir, chdir error for %s\n", argv[1]);
		exit(1);
	}

	while ((dentry = readdir(dirp)) != NULL) {//디렉토리에 있는 파일 읽는다.
		if (dentry->d_ino == 0)
			continue;

		memcpy(filename, dentry->d_name, DIRECTORY_SIZE);//방금 디렉토리에서 읽은 파일명을 filename에 복사

		if (stat(filename, &statbuf) == -1) {//파일 정보를 받는다
			fprintf(stderr, "stat error for %s\n", filename);
			break;
		}

		if ((statbuf.st_mode & S_IFMT) == S_IFREG) //파일이 regular 파일이면
			printf("%-14s %ld\n", filename, statbuf.st_size); //그 파일의 이름과 사이즈 출력
		else
			printf("%-14s\n", filename); //아니면 파일 이름만 출력
	}

	exit(0);
}
