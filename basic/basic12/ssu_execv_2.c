#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(void)
{
	char *argv[] = {
	"ssu_execl_test_1", "param1", "param2", (char *)0
	};
	printf("this is the original program\n");
	// ./ssu_execl_test_1을 exec하는데 위에서 정의한 argv 인자로 넘겨준다.
	execv("./ssu_execl_test_1", argv); 
	//exec해서 프로세스 넘어갔기 때문에 이 프린트문은 출력하지 않는다.
	printf("%s\n", "This line should never get printed\n"); 
	exit(0);
}
