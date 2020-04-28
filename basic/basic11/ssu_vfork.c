#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

void ssu_local_func(void);

int main(void)
{
	printf("Befire vfork\n");
	ssu_local_func();
	printf("After ssu_local_func, my PID is %d\n", getpid());
	exit(0); //자식이 exit()하고 나서 부모 프로세스가 멈춰있다가 이어가게 된다.
}

void ssu_local_func(void) {
	pid_t pid;
	
	//자식 프로세스의 경우
	if ((pid = vfork()) == 0)
		printf("I'm child, My PID is %d\n", getpid());
	//부모 프로세스의 경우
	else if (pid > 0)
		sleep(3);
	//vfork 에러의 경우
	else
		fprintf(stderr, "vfork error\n");
}
