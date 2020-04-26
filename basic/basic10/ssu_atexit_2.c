#include <stdio.h>
#include <stdlib.h>

void ssu_out(void);

int main(void)
{
	if (atexit(ssu_out)) { //exit() 호출하면 ssu_out 자동 호출
		fprintf(stderr, "atexit error\n");
		exit(1);
	}
	exit(0);
}

void ssu_out(void) {
	printf("atexit succeeded!\n");
}
