#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>

int main(void)
{
	struct stat statbuf;
	char *fname = "ssu_myfile";
	int fd;
	

	//읽기쓰기 모드로 파일 생성하고 오픈, _rw_______ 권한을 부여
	if((fd = open(fname,O_RDWR|O_CREAT,0600)) < 0){
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}

	close(fd); //파일 닫고
	stat(fname, &statbuf); //닫은 파일의 정보 담고있는 stat구조체 받아옴
	//파일의 userid와 groupid를 표시
	printf("# 1st stat call # UID: %d GID:%d\n", statbuf.st_uid, statbuf.st_gid);

	//파일의 userid와 groupid를 변경한다.
	if(chown(fname, 501, 300) < 0) {
		fprintf(stderr, "chown error for %s\n", fname);
		exit(1);
	}
	
	stat(fname, &statbuf);//변경된 파일의 정보를 다시 불러오고
	//파일의 userid와 groupid를 표시한다.
	printf("# 2nd stat call # UID:%d GID:%d\n", statbuf.st_uid,statbuf.st_gid);

	//파일이 닫혀있을 경우 그 파일 삭제
	if(unlink(fname) < 0) {
		fprintf(stderr, "unlink error for %s\n", fname);
		exit(1);
	}
	exit(0);
}
