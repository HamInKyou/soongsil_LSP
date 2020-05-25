#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	struct flock lock;
	int fd;
	char command[100];
	
	//첫번째 인자로 입력한 파일을 오픈한다.
	if((fd = open(argv[1], O_RDWR)) == -1) {
		perror(argv[1]);
		exit(1);
	}
	
	//락을 설정하기 위한 과정
	lock.l_type = F_WRLCK; //읽고, 쓰기를 차단시켜준다.
	lock.l_whence = 0;
	lock.l_start = 0l;
	lock.l_len = 0l;
	//파일에 락을 설정해준다.
	if(fcntl(fd, F_SETLK, &lock) == -1) {
		if (errno == EACCES) {
			printf("%s busy -- try later\n", argv[1]);
			exit(2);
		}
		perror(argv[1]);
		exit(3);
	}
	
	//vim argv[1] 꼴로 커맨드 문자열에 저장시켜준다.
	sprintf(command, "vim %s\n", argv[1]);
	//커맨드 문자열에 저장된 내용 실행시켜준다.
	system(command); //vim으로 argv[1] 열어준다.
	//락을 해제해준다.
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &lock);
	close(fd);

	return 0;

}
