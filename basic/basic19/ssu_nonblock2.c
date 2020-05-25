#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>

#define MAX_CALL 100
#define MAX_BUF 20000

void serror(char *str)
{
	fprintf(stderr, "%s\n", str);
	exit(1);
}

int main(void)
{
	int nread, nwrite, val, i = 0;
	char *ptr;
	char buf[MAX_BUF];

	int call[MAX_CALL];

	//표준 입력에 대하여 읽는다.
	nread = read(STDIN_FILENO, buf, sizeof(buf));
	fprintf(stderr, "read %d bytes\n", nread);
	
	//표준 출력의 속성을 갖고온다.
	if ((val = fcntl(STDOUT_FILENO, F_GETFL, 0)) < 0)
		serror("fcntl F_GETFL error");

	//논블럭 속성을 켜준다.	
	val |= O_NONBLOCK;

	//표준 출력에 논블럭 속성을 켜준 val을 적용시켜준다.
	if (fcntl(STDOUT_FILENO, F_SETFL, val) < 0)
		serror("fcntl F_SETF: error");
	
	for (ptr = buf; nread > 0; i++) {
		errno = 0;
		//버퍼의 내용을 출력해준다.
		nwrite = write(STDOUT_FILENO, ptr, nread);
		
		//한번에 얼만큼 읽었는지 저장해준다.
		if (i < MAX_CALL)
			call[i] = nwrite;
		
		fprintf(stderr, "nwrite = %d, errno = %d\n", nwrite, errno);
		if (nwrite > 0) {
			ptr += nwrite;
			nread -= nwrite;
		}
	}
	
	//표준 출력의 속성을 갖고온다.
	if ((val = fcntl(STDOUT_FILENO, F_GETFL, 0)) < 0)
		serror("fcntl F_GETFL error");
	//표준 출력에 논블럭 속성을 꺼준다.
	val &= ~O_NONBLOCK;
	//표준 출력에 논블럭 속성을 꺼준 val을 적용시켜준다.
	if (fcntl(STDOUT_FILENO, F_SETFL, val) < 0)
		serror("fcntl F_SETFL error");
	
	//한번에 얼마씩 읽었는지 출력해준다.
	for (i = 0; i < MAX_CALL; i++)
		fprintf(stdout, "call[%d] = %d\n", i, call[i]);

	exit(0);
}
