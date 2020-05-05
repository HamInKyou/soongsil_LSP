#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>

int ssu_daemon_init(void);

int main(void)
{
	pid_t pid;

	pid = getpid(); //현재 프로세스 id 받는다.
	printf("parent process : %d\n", pid); //부모 프로세스 id 출력
	printf("daemon process initialization\n");

	//디몬 프로세스 생성
	if (ssu_daemon_init() < 0) {
		fprintf(stderr, "ssu_daemon_init failed\n");
		exit(1);
	}
	exit(0);
}

int ssu_daemon_init(void) {
	pid_t pid;
	int fd, maxfd;

	//자식 프로세스 생성
	if ((pid = fork()) < 0) { //에러의 경우
		fprintf(stderr, "fork error\n");
		exit(1);
	} 
	//부모의 경우
	else if (pid != 0)
		exit(0); //규칙1. 백그라운드로 돌리기 위해 부모를 죽인다.
	
	//자식의 경우
	pid = getpid(); //자식 프로세스 id 받는다.
	printf("process %d running as daemon\n", pid);
	setsid(); //규칙2. 새로운 프로세스 그룹 생성하고 자신이 그룹 리더가 된다.
	signal(SIGTTIN, SIG_IGN); //규칙3.
	signal(SIGTTOU, SIG_IGN); //작업제어와 연관된 시그널
	signal(SIGTSTP, SIG_IGN); //이 세개를 무시한다.
	
	//규칙6. 오픈되는 모든 파일 디스크립터를 닫는다.
	maxfd = getdtablesize(); //파일 디스크립터 최대로 생성할 수 있는 사이즈
	for (fd = 0; fd < maxfd; fd++)
		close(fd);
	

	umask(0); //규칙4. 파일모드 생성 마스크를 해제한다. -> 디몬이 생성할 파일의 접근 허가 모드를 모두 허용
	chdir("/"); //규칙5. 현재 작업 디렉토리를 루트 디렉토리로 설정한다.

	//규칙7. 표준 입출력과 표준 에러를 /dev/null로 재지정한다.
	fd = open("/dev/null", O_RDWR); //위에서 다 닫았으니 /dev/null이 0번째 디스크립터로
	dup(0); //1번 디스크립터는 0번째 디스크립터를 dup한거
	dup(0); //2번 디스크립터는 0번째 디스크립터를 dup한거
	return 0;
}

