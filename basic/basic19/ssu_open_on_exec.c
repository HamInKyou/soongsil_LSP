#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(void)
{
	int fd;
	int val;
	
	//exec_copy.txt라는 파일을 생성하고 연다.
	fd = open("exec_copy.txt", O_CREAT);
	
	//무한루프 프로그램인 ./loop를 실행시킨다.
	execl("/home/ingyu/LSP/basic/basic19/loop", "./loop", NULL);
	exit(0);
}
