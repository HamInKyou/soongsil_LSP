#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	printf("this is the original program\n");
	//명령행 인자들을 execl의 인자들로 넘겨주고, 마지막 인자는 널문자로 해준다.
	execl("./ssu_execl_test_1", "ssu_execl_test_1", "param1", "param2", "param3", (char *)0);
	//execl 수행으로 인하여 프로세스가 넘어갔으므로 아래의 프린트문은 출력하지 못한다.
	printf("%s\n", "this line should never get printed\n");
	exit(0);
}
