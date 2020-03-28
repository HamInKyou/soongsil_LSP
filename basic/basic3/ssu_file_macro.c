#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>

int main(int argc, char *argv[])
{
	struct stat file_info; //파일 정보 담기위한 stat 구조체
	char *str; //파일의 모드 정보를 출력하기 위해 선언
	int i;

	//입력 인자로 들어온 것 하나씩 검사
	for (i = 1; i < argc; i++) {
		printf("name = %s, type = ", argv[i]);
		//인자로 들어온 파일의 정보를 stat 구조체에 저장
		if(lstat(argv[i], &file_info) < 0) {
			fprintf(stderr, "lstat error\n");
			continue;
		}
		//파일의 모드 검사
		if (S_ISREG(file_info.st_mode))//일반파일?
			str = "regular";
		else if(S_ISDIR(file_info.st_mode))//디렉토리 파일?
			str = "directory";
		else if(S_ISCHR(file_info.st_mode)) //문자 디바이스?
			str = "character special";
		else if(S_ISBLK(file_info.st_mode)) //블록 디바이스?
			str = "block special";
		else if(S_ISFIFO(file_info.st_mode)) //FIFO, Pipe 파일?
			str = "FIFO";
		else if(S_ISLNK(file_info.st_mode)) //소켓 파일?
			str = "symbolic link";
		else if(S_ISSOCK(file_info.st_mode)) //심볼링 링크?
			str = "socket";
		else //그 외 나머지
			str = "unknown mode";
		printf("%s\n", str);
	}
	exit(0);
}
