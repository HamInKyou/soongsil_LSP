#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main(void)
{
	sigset_t set; //sigset 선언

	sigemptyset(&set); //sigset을 다 비운다.
	sigaddset(&set, SIGINT); //SIGINT만 sigset에 추가해준다.

	switch(sigismember(&set, SIGINT)){ //SIGINT가 sigset에 속하는지?
		case 1: 
			printf("SIGINT is included. \n");
			break;
		case 0 :
			printf("SIGINT is not included. \n");
			break;
		default :
			printf("failed to call sigismember() \n");
	}

	switch (sigismember(&set, SIGSYS)){ //SIGSYS가 sigset에 속하는지?
		case 1:
			printf("SIGSYS is included. \n");
			break;
		case 0 :
			printf("SIGSYS is not included. \n");
			break;
		default :
			printf("failed to call sigismember() \n");
	}
	exit(0);
}
