#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/resource.h>

#define ssu_show(limit) ssu_show_limit(limit, #limit)

void ssu_show_limit(int rlim_type, char *rlim_name);

int main(void)
{
	printf("      resource soft limit hard limit    \n\n");
	ssu_show(RLIMIT_CPU); //프로세스가 소비할 수 있는 최대 CPU 시간
	ssu_show(RLIMIT_FSIZE); //파일 생성 시 파일의 최대 크기
	ssu_show(RLIMIT_DATA); //자료 구역 전체의 최대 크기
	ssu_show(RLIMIT_STACK); //스택 최대 크기
	ssu_show(RLIMIT_CORE); //한 코어 파일의 최대 크기

#ifdef RLIMIT_RSS
	ssu_show(RLIMIT_RSS); //메모리 상주 세트 크기의 최댓값
#endif
#ifdef RLIMIT_MEMLOCK
	ssu_show(RLIMIT_MEMLOCK); //한 프로세스가 mlock으로 잠글 수 있는 메모리 최대 용량
#endif
#ifdef RLIMIT_NPROC
	ssu_show(RLIMIT_NPROC); //실제 사용자 ID당 최대 자식 프로세스 개수
#endif
#ifdef RLIMIT_OFILE
	ssu_show(RLIMIT_OFILE);  //RLIMIT_NOFILE에 대한 BSD 이름
#endif
#ifdef RLIMIT_NOFILE
	ssu_show(RLIMIT_NOFILE); //한 프로세스가 열어둘 수 있는 최대 파일 개수
#endif
#ifdef RLIMIT_VMEN
	ssu_show(RLIMIT_VMEN); //한 프로세스의 총 자유 메모리 용량의 최댓값
#endif

	exit(0);
}


void ssu_show_limit(int rlim_type, char *rlim_name) {
	struct rlimit rlim;
	char cur[11], max[11];
	
	//자원 한계를 가져온다.
	getrlimit(rlim_type, &rlim);
	
	//약한 한계가 무한대일 경우
	if(rlim.rlim_cur == RLIM_INFINITY)
		strcpy(cur, "infinity");
	else 
		sprintf(cur, "%10ld", rlim.rlim_cur);
	
	//강한 한계가 무한대일 경우
	if(rlim.rlim_max == RLIM_INFINITY)
		strcpy(max, "infinity");
	else
		sprintf(max, "%10ld", rlim.rlim_max);
	
	printf("%15s : %10s %10s\n", rlim_name, cur, max);
}
