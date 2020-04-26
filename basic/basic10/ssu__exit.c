#include <stdio.h>
#include <unistd.h>

int main(void)
{
	printf("Good afternoon?");
	_exit(0); //버퍼에 남은거 찍지도 않고 종료
}
