#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
	pid_t child1, child2;
	int pid, status;
	
	//자식 프로세스 생성, date 명령 실행
	if ((child1 = fork()) == 0)
		execlp("date", "date", (char *)0);
	
	//자식 프로세스 생성, who 명령 실행
	if ((child2 = fork()) == 0)
		execlp("who", "who", (char *)0);

	printf("parent: waiting for children\n");
	
	//자식 프로세스 모두 끝날 때까지 반복, 자식 종료상태 status에 저장
	while ((pid = wait(&status)) != -1) {
		if (child1 == pid) //종료상태 출력
			printf("parent: first child: %d\n", (status >> 8));
		else if (child2 == pid) //종료상태 출력
			printf("parent: second child: %d\n", (status >> 8));
	}

	printf("parent: all children terminated\n");
	exit(0);
}
