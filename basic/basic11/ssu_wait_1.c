#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void ssu_echo_exit(int status);

int main(void)
{
	pid_t pid;
	int status;
	
	//fork를 통해 자식 프로세스 생성
	if ((pid = fork()) < 0) { //에러일 경우
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	//자식 프로세스의 경우
	else if (pid == 0)
		exit(7); //종료상태 보냄
	
	//자식프로세스 끝날때까지 기다렸다가
	//자식 프로세스 종료상태 받는다
	if (wait(&status) != pid) { //wiat 에러의 경우
		fprintf(stderr, "wait error\n");
		exit(1);
	}

	ssu_echo_exit(status);
	
	//fork를 통해 자식 프로세스 생성
	if ((pid = fork()) < 0) { //에러일 경우
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if (pid == 0)
		abort(); //비정상적 종료 야기
	
	//자식프로세스 끝날때까지 기다렸다가
	//자식 프로세스 종료상태 받는다.
	if (wait(&status) != pid) { //wait 에러의 경우
		fprintf(stderr, "wait error\n");
		exit(1);
	}

	ssu_echo_exit(status);

	//자식프로세스 끝날때까지 기다렸다가
	//자식 프로세스 종료상태 받는다.
	if ((pid = fork()) < 0) { //에러일 경우
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if (pid == 0)
		status /= 0; //0으로 나눔 -> 비정상적 종료

	if (wait(&status) != pid) { //wait 에러의 경우
		fprintf(stderr, "wait error\n");
		exit(1);
	}

	ssu_echo_exit(status);
	exit(0);
}

//자식프로세스로부터 받은 종료 상태 출력해주는 함수
void ssu_echo_exit(int status) {
	if (WIFEXITED(status)) //자식 프로세스가 정상적으로 종료했을 경우
		printf("normal termination, exit status = %d\n", WEXITSTATUS(status)); //하위8비트(종료상태) 출력
	else if (WIFSIGNALED(status)) //비정상적 종료의 경우 받은 시그널 번호 출력
		printf("abnormal termination,  signal number = %d%s\n", WTERMSIG(status),
#ifdef WCOREDUMP
				WCOREDUMP(status) ? " (core file generated)" : "");
#else
				"");
#endif
	else if (WIFSTOPPED(status)) //자식 프로세스가 현재 중지상태일 경우 중단시킨 시그널 번호 리턴
		printf("chile stopped, signal number = %d\n", WSTOPSIG(status));
}

