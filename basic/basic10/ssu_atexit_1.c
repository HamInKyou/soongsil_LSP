#include <stdio.h>
#include <stdlib.h>

static void ssu_exit1(void);
static void ssu_exit2(void);

int main(void)
{
	if(atexit(ssu_exit2) != 0) { //exit() 호출시 자동으로 ssu_exit2 호출하게
		fprintf(stderr, "atexit error for ssu_exit2");
		exit(1);
	}
	if(atexit(ssu_exit1) != 0) { //exit() 호출시 자동으로 ssu_exit1 호출하게
		fprintf(stderr, "atexit error for ssu_exit1");
		exit(1);
	}
	if(atexit(ssu_exit1) != 0) { //exit() 호출시 자동으로 ssu_exit1 호출하게
		fprintf(stderr, "atexit error for ssu_exit1");
		exit(1);
	}
	printf("done\n");
	exit(0); //atexit()를 아래서부터 역순으로 호출해준다.
}

static void ssu_exit1(void) {
	printf("ssu_exit1 handler\n");
}
static void ssu_exit2(void) {
	printf("ssu_exit2 handler\n");
}
