#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/stat.h>

//_rw_rw_rw 권한 (0666)
#define RW_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

int main(void)
{
	char *fname1 = "ssu_file1";
	char *fname2 = "ssu_file2";

	umask(0); //umask 0으로 없애준다.

	//위에서 선언한 권한으로 파일 생성
	//umask가 0이기에 생성되는 파일의 권한은 _rw_rw_rw이다.
	if (creat(fname1, RW_MODE) < 0) {
		fprintf(stderr, "creat error for %s\n", fname1);
		exit(1);
	}

	
	umask(S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH); //umask ___rw_rw_로 설정

	//umask가 ___rw_rw_이기에 RW_MODE와 XOR연산을 하여 _rw_______로 생성한다.
	if(creat(fname2, RW_MODE) < 0) {
		fprintf(stderr, "creat error for %s\n", fname2);
		exit(1);
	}
	
	exit(0);
}
