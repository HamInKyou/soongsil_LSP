#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void ssu_signal_handler(int signo);

void (*ssu_func)(int);

int main(void)
{
	//시그널 핸들러 등록
	ssu_func =signal(SIGINT, ssu_signal_handler); //Ctrl+c 누르면 ssu_signal_handler 실행

	while(1) { 
		printf("process running....\n");
		sleep(1); //1초마다 위의 프린트문 출력
	}
	exit(0);
}

//ctrl+c 누르면 실행하는 함수
void ssu_signal_handler(int signo) {
	printf("SIGINT 시그널 발생.\n");
	printf("SIGINT를 SIG_DFL로 재설정 함.\n");
	signal(SIGINT, ssu_func); //시그널에 대한 이전 핸들러 호출 (디폴트)
}
