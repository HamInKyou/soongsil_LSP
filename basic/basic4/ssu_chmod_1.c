#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>

int main(void)
{
	struct stat statbuf;
	char *fname1 = "ssu_file1";
	char *fname2 = "ssu_file2";

	if(stat(fname1, &statbuf) < 0)
		fprintf(stderr,"stat error %s\n", fname1);
	
	//ssu_file1의 stat의 멤버 변수 st_mode와 not(S_IXGRP)를 And
	//st_mode에서 _rwxrw_rwx와 겹치는거 살리고 나머지 다 0으로
	//set uid를 킨다.
	if(chmod(fname1, (statbuf.st_mode & ~S_IXGRP) | S_ISUID) < 0)
		fprintf(stderr, "chmod error %s\n", fname1);
	
	//ssu_file2의 접근 권한을  _rw_r__r_x로 설정해준다.
	if(chmod(fname2, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IXOTH) < 0)
		fprintf(stderr, "chmod error %s\n", fname2);

	exit(0);
}
