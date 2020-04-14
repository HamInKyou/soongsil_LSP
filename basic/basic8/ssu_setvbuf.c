#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void ssu_setbuf(FILE *fp, char *buf);

int main(void)
{
	char buf[BUFFER_SIZE];
	char *fname = "/dev/pts/1";
	FILE *fp;
	
	//파일을 쓰기모드로 오픈한다.
	//해당 터미널에 출력을 하겠다.
	if ((fp = fopen(fname, "w")) == NULL) {
		fprintf(stderr, "fopen error for %s", fname);
		exit(1);
	}

	ssu_setbuf(fp, buf); //터미널에 따라 버퍼를 설정해준다.
	fprintf(fp, "Hello, ");
	sleep(1);
	fprintf(fp, "UNIX!!");
	sleep(1);
	fprintf(fp, "\n");
	sleep(1);
	ssu_setbuf(fp, NULL); //논버퍼 모드로 버퍼를 설정해준다.
	fprintf(fp, "HOW");
	sleep(1);
	fprintf(fp, " ARE");
	sleep(1);
	fprintf(fp, " YOU?");
	sleep(1);
	fprintf(fp, "\n");
	sleep(1);
	exit(0);
}

void ssu_setbuf(FILE *fp, char *buf) {
	size_t size;
	int fd;
	int mode;
	fd = fileno(fp); //fp와 관련된 파일 디스크립터 얻는다.
	
	if (isatty(fd))  //그 파일이 터미널을 사용하는지
		mode = _IOLBF; //사용한다면 모드를 개행에 따라 입출력을 수행하는 라인버퍼모드로
	else
		mode = _IOFBF; //사용하지 않는다면 버퍼가 꽉 찼을때 입출력을 수행하는 풀버퍼모드로
	
	if (buf == NULL) { //버퍼가 할당되지 않았다면
		mode = _IONBF; //버퍼를 사용하지 않는 논버퍼모드로
		size = 0;      //버퍼의 크기를 0으로
	}
	else //버퍼가 할당되었다면
		size = BUFFER_SIZE; //버퍼의 크기를 정해준다.

	//버퍼 모드에 맞게 버퍼를 설정해준다.
	setvbuf(fp, buf, mode, size);
}

