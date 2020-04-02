#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

int main(void)
{
	char buf[64];
	char *fname = "ssu_tempfile";
	int fd;
	int length;
	
	//파일을 읽기쓰기 모드로 여는데 없으면 생성, 있으면 내용 지우고 새로
	if ((fd = open(fname, O_RDWR | O_CREAT | O_TRUNC, 0600)) < 0) {
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}

	//연결링크 카운터를 감소시킨다. 아직 오픈된 상태이므로 삭제는 안됨
	if (unlink(fname) < 0){
		fprintf(stderr, "unlink error for %s\n", fname);
		exit(1);
	}

	if (write(fd, "How are you?", 12) != 12) {
		fprintf(stderr, "write error\n");
		exit(1);
	}
	//파일의 시작위치로 프리셋 이동
	lseek(fd, 0, 0);
	//파일의 내용 읽기
	if((length = read(fd, buf, sizeof(buf))) < 0) {
		fprintf(stderr, "buf read error\n");
		exit(1);
	}
	else //읽은 내용 끝에 널문자를 추가해줌
		buf[length] = 0;

	printf("%s\n", buf);

	//파일 닫는다, 이 순간 삭제
	close(fd);

	//다시 그 파일을 읽기쓰기 모드로 열려고 해도 지워졌기 때문에 에러
	if ((fd = open(fname, O_RDWR)) < 0) {
		fprintf(stderr, "second open error for %s\n", fname);
		exit(1);
	}

	else
		close(fd);
	
	exit(0);
}
