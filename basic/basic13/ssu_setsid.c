#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	pid_t pid;
	
	//자식 프로세스 생성
	if((pid = fork()) < 0) { //에러인 경우 
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if(pid != 0) //부모프로세스일 경우
		exit(1); //끝내버린다.

	printf("before pid = %d, sid = %d\n", getpid(), getsid(getpid()));
	setsid(); //새 세션을 생성한다. 자식이 세션 리더가 된다.
	printf("after pid = %d, sid = %d\n", getpid(), getsid(getpid()));
	exit(0);
}
