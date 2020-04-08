#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <utime.h>

int main(int argc, char *argv[])
{
	struct utimbuf time_buf; //시간정보 저장하는 구조체
	struct stat statbuf; //파일의 정보 저장하는 구조체
	int fd;
	int i;

	for (i = 1; i < argc; i++) {
		if (stat(argv[i], &statbuf) < 0) { //파일정보 받아오는데 에러 떴을경우
			fprintf(stderr, "stat error for %s\n", argv[i]);
			continue;
		}

		if ((fd = open(argv[i], O_RDWR | O_TRUNC)) < 0) { //읽고쓰기모드로 오픈, 내용 있으면 다 지우고
			fprintf(stderr, "open error for %s\n", argv[i]);
			continue;
		}

		close(fd); //파일 닫는다.
		time_buf.actime = statbuf.st_atime; //파일의 최종 접근 시간 받기
		time_buf.modtime = statbuf.st_mtime; //파일의 최종 변경 시간 받기

		if (utime(argv[i], &time_buf) < 0) { //파일의 최종 접근시간과 변경시간을 방금 저장한 값으로 변경
			fprintf(stderr, "utime error for %s\n", argv[i]);
			continue;
		}
	}
}
