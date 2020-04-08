#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "ssu_score.h"

#define SECOND_TO_MICRO 1000000

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);

int main(int argc, char *argv[])
{
	struct timeval begin_t, end_t;
	gettimeofday(&begin_t, NULL);//시간 측정 시작

	ssu_score(argc, argv);

	gettimeofday(&end_t, NULL);//시간 측정 끝
	ssu_runtime(&begin_t, &end_t); //실행시간 출력

	exit(0);
}

//실행시간 측정하는 함수
void ssu_runtime(struct timeval *begin_t, struct timeval *end_t)
{
	end_t->tv_sec -= begin_t->tv_sec; //측정 완료시간(초단위)에서 시작시간(초단위) 빼서 최신화

	if(end_t->tv_usec < begin_t->tv_usec){ //측정 완료시간(micro단위)이 측정 시작시간(micro단위)보다 작을경우
		end_t->tv_sec--; //측정 완료시간(초단위)에서 1초 빼준다.
		end_t->tv_usec += SECOND_TO_MICRO; //측정 완료시간(micro단위)에 1초 더해준다.
	}

	end_t->tv_usec -= begin_t->tv_usec; //측정 완료시간(micro단위)에서 시작시간(micro단위) 빼서 최신화
	printf("Runtime: %ld:%06ld(sec:usec)\n", end_t->tv_sec, end_t->tv_usec); //초단위,micro단위까지 출력
}
