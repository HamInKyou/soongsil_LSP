#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
	//자식 프로세스 생성
	if(fork() == 0) //"/bin/echo" 실행, echo부터 0번째 인자 (echo this is message one 친거)
		execl("/bin/echo", "echo", "this is", "message one", (char *)0);
	
	//자식 프로세스 생성
	if(fork() == 0) //"/bin/echo" 실행, echo부터 0번째 인자
		execl("/bin/echo", "echo", "this is", "message Two", (char *)0);
	
	printf("parent: waiting for children\n");
	
	//자식 프로세스 모두 끝날 때까지 기다린다.
	while (wait((int*)0) != -1);
	
	printf("parent: all children terminated\n");
	exit(0);
}
