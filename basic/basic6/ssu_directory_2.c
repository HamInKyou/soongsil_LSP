#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>

#ifdef PATH_MAX
static int pathmax = PATH_MAX;
#else
static int pathmax = 0;
#endif

#define MAX_PATH_GUESSED 1024

#ifdef LINE_MAX
#define LINE_MAX 2048
#endif

char *pathname;
char command[LINE_MAX], grep_cmd[LINE_MAX];

int ssu_do_grep(void) {
	struct dirent *dirp; //directory 구조체 가리키기 위해
	struct stat statbuf; //파일 정보 받는 구조체
	char *ptr;
	DIR *dp; //directory 가리키는 포인터

	if (lstat(pathname, &statbuf) < 0) { //pathname에 들어온 파일 정보 저장
		fprintf(stderr, "lstat error for %s\n", pathname);
		return 0;
	}

	if (S_ISDIR(statbuf.st_mode)== 0) { //파일이 디렉토리일 경우
		sprintf(command, "%s %s", grep_cmd, pathname); //command 배열에 grep 명령과 pathname 차례로 저장한다.
		printf("%s : \n", pathname);                   //grep 명령은 파일로부터 문자열 검색하는 것.
		system(command); //저장한 명령을 실행한다.
		return 0;
	}

	ptr = pathname + strlen(pathname); //포인터를 pathname 끝 가리키게 하고
	*ptr++ = '/'; //끝에 /붙이고
	*ptr = '\0'; //'\0\으로 마무리

	if ((dp = opendir(pathname)) == NULL) {//디렉토리 열기
		fprintf(stderr, "opendir error for %s\n", pathname);
		return 0;
	}

	while ((dirp = readdir(dp)) != NULL){ //디렉토리의 파일 읽는다.
		if (strcmp(dirp->d_name, ".") && strcmp(dirp->d_name, ".."))  { //'.','..'읽고 나서
			strcpy(ptr, dirp->d_name); //pathname 뒤에 파일 또 추가한다(경로 잇기)

			if (ssu_do_grep() < 0) //그 상태에서 또 ssu_do_grep() 명령 실행한다.
				break;
		}
	}
	ptr[-1] = 0;
	closedir(dp);
	return 0;
}

void ssu_make_grep(int argc, char *argv[]) {
	int i;
	strcpy(grep_cmd, " grep"); // grep_cmd에 " grep" 복사

	for(i = 1; i < argc-1 ; i++) {
		strcat(grep_cmd, " "); //grep_cmd 뒤에 space 하나 붙인다.
		strcat(grep_cmd, argv[i]); //현재 차례의 인자를 뒤에 붙인다.
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "usage: %s <-CVbchilnsvwx> <-num> <-A num> <-B num> <-f file> \n"
							 "<-e> expr <directoty>\n", argv[0]);
		exit(1);
	}

	if (pathmax == 0){
		if ((pathmax = pathconf("/", _PC_PATH_MAX)) < 0) //첫번째 인자로 들어온 경로의 경로이름의 최대 바이트수 받기
			pathmax = MAX_PATH_GUESSED; //알아낸 패스명 최대 바이트 수보다 하나 크게 저장
		else
			pathmax++;
	}
	//패스명의 최대바이트 수보다 하나 크게 동적할당
	if ((pathname = (char *)malloc(pathmax+1)) == NULL) {
		fprintf(stderr, "malloc error\n");
		exit(1);
	}

	strcpy(pathname, argv[argc-1]); //pathname에 마지막 인자 복사
	ssu_make_grep(argc, argv);
	ssu_do_grep();
	exit(0);
}
