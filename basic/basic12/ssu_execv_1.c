#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/wait.h>

double ssu_maketime(struct timeval *time);
void term_stat(int stat);
void ssu_print_child_info(int stat, struct rusage *rusage);

int main(void)
{
	struct rusage rusage;
	pid_t pid;
	int status;
	
	//자식 프로세스 생성, 자식인 경우
	if ((pid = fork()) == 0) {
		char *args[] = {"find", "/", "-maxdepth", "4", "-name", "stdio.h", NULL};
		if (execv("/usr/bin/find", args) < 0) { // /usr/bin/find exec하는데 args 인자로 해서
			fprintf(stderr, "exev error\n");
			exit(1);
		}
	}
	//자식 프로세스 종료까지 기다리는데, 
	//종료된 프로세스에 대한 정보를 status에 자원사용에 관한 정보를 rusage에
	if (wait3(&status, 0, &rusage) == pid)
		ssu_print_child_info(status, &rusage); //자식 프로세스의 정보를 출력하는 함수 호출
	else {
		fprintf(stderr, "wait3 error\n");
		exit(1);
	}
	exit(0);
}

//시간 관련 구조체를 인자를 받아와 시간을 만들어주는 함수
double ssu_maketime(struct timeval *time) {
	return ((double)time -> tv_sec + (double)time -> tv_usec/1000000.0);
}


//인자로 넘겨준 프로세스의 종료상태 출력해주는 함수
void term_stat(int stat) {
	if (WIFEXITED(stat)) //정상적으로 종료된 프로세스일 경우
		printf("normally terminated. exit status = %d\n", WEXITSTATUS(stat));
	else if (WIFSIGNALED(stat)) //비정상적으로 종료된 프로세스일 경우
		printf("abnormal termination by signal %d. %s\n", WTERMSIG(stat),
#ifdef WCOREDUMP
				WCOREDUMP(stat)?"core dumped":"no core"
#else
				NULL
#endif
				);
	else if (WIFSTOPPED(stat)) //중지된 프로세스일 경우
		printf("stopped by signal %d\n", WSTOPSIG(stat));
}

//종료된 프로세스 정보와 자원 사용에 대한 출력을 하는 함수
void ssu_print_child_info(int stat, struct rusage *rusage) {
	printf("Termination info follows\n");
	term_stat(stat); //인자로 넘겨준 프로세스의 종료상태 출력해주는 함수 호출
	printf("user CPU time : %.2f(sec)\n", ssu_maketime(&rusage->ru_utime));
	printf("system CPU time : %.2f(sec)\n", ssu_maketime(&rusage->ru_stime));
}


