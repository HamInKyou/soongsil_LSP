#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>

#define MODE_EXEC (S_IXUSR|S_IXGRP|S_IXOTH) //___x__x__x

int main(int argc, char *argv[])
{
	struct stat statbuf;
	int i;
	if (argc < 2) {
		fprintf(stderr, "usage: %s <file1> <file2> ... <fileN>\n", argv[0]);
		exit(1);
	}
	
	for (i = 1; i < argc; i++){ 
		if(stat(argv[i], &statbuf) < 0) {
			fprintf(stderr, "%s : stat error\n", argv[i]);
			continue;
		}

		//현재 모드에 실행 권한을 다 켜준다.
		statbuf.st_mode |= MODE_EXEC;
		//현재 모드에서 group과 other의 실행권한이
		//켜져있을 경우 꺼주고 꺼져있을 경우 켜준다.
		statbuf.st_mode ^= (S_IXGRP|S_IXOTH);

		//위에서 저장한 모드대로 변경해준다.
		if(chmod(argv[i], statbuf.st_mode) < 0)
			fprintf(stderr, "%s : chmod error\n", argv[i]);
		else
			printf("%s : file permission was changed. \n", argv[i]);
	}
	exit(0);
}
