#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>

struct stat statbuf; 

//파일의 최종수정시간정보와 파일 존재 유무를 체크하는 함수선언
void ssu_checkfile(char *fname, time_t *time);
int main(int argc, char *argv[])
{
	time_t intertime; //시간 정보를 저장하는 변수
	
	//입력이 제대로 잘 되었는지?
	if (argc !=2) {
		fprintf(stderr, "usage : %s <file>\n", argv[0]);
		exit(1);
	}

	//인자로 들어온 파일의 정보를 갖고있는 stat구조체를 저장한다.
	if (stat(argv[1],&statbuf) < 0) {
		fprintf(stderr, "stat error for %s\n", argv[1]);
		exit(1);
	}
	
	//파일의 최종 수정시간 정보를 저장한다.
	intertime = statbuf.st_mtime;
	//10msec 간격으로 파일의 변경사항을 체크한다.
	while(1) {
		ssu_checkfile(argv[1], &intertime);
		sleep(10);
	}
}

void ssu_checkfile(char *fname, time_t *time){
	if ( stat(fname, &statbuf) < 0) { //파일의 정보를 최신화
		fprintf(stderr, "Warning : ssu_checkfile() error!\n");
		exit(1);
	}

	//저장된 최종수정시간 정보와 최신화된 파일의 최종수정시간 정보가 다를경우
	else if ( statbuf.st_mtime != *time) {
		printf("Warning : %s was modified!.\n", fname);
		*time = statbuf.st_mtime; //저장된 최종수정시간을 최신화
	}
}

