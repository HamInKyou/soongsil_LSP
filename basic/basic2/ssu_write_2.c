#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

#define S_MODE 0644 //유저에겐 읽기,쓰기 그룹에겐 읽기 타인에겐 읽기 권한 부여
#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
	char buf[BUFFER_SIZE];
	int fd1, fd2; //파일 디스크립터 두개 선언
	int length;

	//입력 파일과 출력 파일을 입력받는다.
	if (argc != 3) {
		fprintf(stderr, "Usage : %s filein fileout\n", argv[0]);
		exit(1);
	}
	
	//입력 파일을 읽기모드로 오픈한다.
	if ((fd1 = open(argv[1],O_RDONLY)) < 0) {
		fprintf(stderr,"open error for %s\n", argv[1]);
		exit(1);
	}

	//출력파일을 쓰기모드로 오픈하는데, 없을 경우 생성하고 있을경우 내용을 다 지운다.
	if ((fd2 = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, S_MODE)) < 0) {
		fprintf(stderr,"open error for %s\n", argv[1]);
		exit(1);
	}
	
	//입력파일에서 버퍼사이즈만큼 읽은 뒤에, 출력파일에 읽은 길이만큼 쓴다.
	while ((length = read(fd1, buf, BUFFER_SIZE)) > 0)
		write(fd2, buf, length);

	exit(0);
}
