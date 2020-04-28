#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char glob_str[] = "write to standart output\n";
int glob_val = 10;

int main(void)
{
	pid_t pid;
	int loc_val;

	loc_val = 100;
	//버퍼링이 일어나지 않아 표준 출력에 바로 쓴다.
	if (write(STDOUT_FILENO, glob_str, sizeof(glob_str)-1) != sizeof(glob_str) -1) {
		fprintf(stderr, "write error\n");
		exit(1);
	}
	
	//라인 버퍼링이 일어나 파일에 출력할 때 아래 프린트문과 같이 출력됨
	//부모 프로세스의 버퍼와 자식 프로세스의 버퍼에 존재,
	//파일에 출력할 경우 부모 프로세스에도 나오고 자식 프로세스에도 나온다.
	printf("before fork\n");

	if((pid = fork()) < 0) { //fork 에러의 경우
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if(pid == 0) { //자식 프로세스일 경우
		glob_val++;
		loc_val++;
	}
	else //부모 프로세스일 경우
		sleep(3);

	printf("pid = %d, glob_val = %d, loc_val = %d\n", getpid(), glob_val, loc_val);
	exit(0);
}
