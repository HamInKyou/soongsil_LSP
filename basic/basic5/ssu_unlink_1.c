#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

int main(void)
{
	char *fname = "ssu_dump.txt";
	if(open(fname, O_RDWR) < 0) { //파일을 읽기쓰기모드로 연다.
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}
	if(unlink(fname) < 0){ //연결링크 카운터 감소시킨다. 오픈된 파일이 없다면 제거한다.
		fprintf(stderr, "unlink error for %s\n", fname);
		exit(1);
	}
	printf("File unlinked\n");
	sleep(20);
	printf("Done\n");
	exit(0); //프로세스 끝나고 나면 파일 삭제됨
}
