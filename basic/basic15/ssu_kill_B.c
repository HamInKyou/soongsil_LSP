#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	while(1) { //프린트문 5초마다 반복해서 출력
		printf("\n[OSLAB]");
		sleep(5);
	}

	exit(0);
}
