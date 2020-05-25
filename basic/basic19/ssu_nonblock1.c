#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

void set_flags(int fd, int flags);
void clr_flags(int fd, int flags);

char buf[500000];

int main(void)
{
	int ntowrite, nwrite;
	char *ptr;
	
	//표준입력을 통해 buf에 저장
	ntowrite = read(STDIN_FILENO, buf, sizeof(buf));
	fprintf(stderr, "reading %d bytes\n", ntowrite);
	
	//표준 출력을 논블록 설정
	set_flags(STDOUT_FILENO, O_NONBLOCK);

	//ptr은 buf를 가리키게
	ptr = buf;

	//읽은게 있다면
	while (ntowrite > 0) {
		errno = 0;
		//표준 출력해 버퍼의 내용 써준다.
		nwrite = write(STDOUT_FILENO, ptr, ntowrite);
		fprintf(stderr, "nwrite = %d, errno = %d\n", nwrite, errno);
		
		//표준 출력으로 쓴만큼 버퍼의 포인터 뒤로 밀어준다.
		if (nwrite > 0) {
			ptr += nwrite;
			ntowrite -= nwrite;
		}
	}
	
	//표준 출력의 논블록 플래그를 꺼준다.
	clr_flags(STDOUT_FILENO, O_NONBLOCK);
	exit(0);
}

void set_flags(int fd, int flags){
	int val;

	//파읠의 속성(플래그)를 갖고 온다.
	if ((val = fcntl(fd, F_GETFL, 0)) < 0) {
		fprintf(stderr, "fcntl F_GETFL failed");
		exit(1);
	}
	
	//val에 flags에 해당하는 플래그 켜준다
	val |= flags;

	//파일의 플래그를 val로 재지정 해준다.
	if (fcntl(fd, F_SETFL, val) < 0) {
		fprintf(stderr, "fcntl F_SETFL failed");
		exit(1);
	}
}

void clr_flags(int fd, int flags)
{
	int val;
	
	//파일의 속성(플래그)를 갖고 온다.
	if((val = fcntl(fd, F_GETFL, 0)) < 0) {
		fprintf(stderr, "fcntl F_GETFL failed");
		exit(1);
	}
	
	//flags에 해당하는 플래그 꺼준다.
	val &= ~flags;
	
	//파일의 플래그를 val로 재지정 해준다.
	if (fcntl(fd, F_SETFL, val) < 0) {
		fprintf(stderr, "fcntl F_SETFL failed");
		exit(1);
	}
}
