#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define EXIT_CODE 1

int main(void)
{
	pid_t pid;
	int ret_val, status;
	
	//자식 프로세스일 경우
	if ((pid = fork()) == 0) {
		printf("child: pid = %d ppid = %d exit_code = %d\n", getpid(), getppid(), EXIT_CODE);
		exit(EXIT_CODE); //종료 상태를 1로 리턴
	}
	
	printf("parent: waiting for childe = %d\n", pid);
	ret_val = wait(&status); //자식 프로세스로부터 종료상태를 받음
	printf("parent: return value = %d, ", ret_val);
	printf(" child's status = %x", status); //16비트 종료상태 출력
	printf(" and shifted = %x\n", (status >> 8)); //shift해서 하위 8비트 (종료 상태) 출력
	exit(0);
}
