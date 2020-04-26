#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <setjmp.h>

void ssu_nested_func(int loc_var, int loc_volatile, int lov_register);

static jmp_buf glob_buffer; //스택환경과 레지스터 환경을 임시로 저장해준다.

int main(void)
{
	register int loc_register; //레지스터 변수
	volatile int loc_volatile; //변수를 최적하에서 제외하여 항상 메모리에 접근하도록
	int loc_var; //지역 변수

	loc_var = 10;
	loc_volatile = 11;
	loc_register = 12;

	if (setjmp(glob_buffer) != 0) { //setjmp를 설정해주고, longjmp에 의해 다시 호출됐을 경우 if문 실행
		//최적화 하지 않으면 longjmp시 값 유지, 최적화 했을 경우 volatile만 그대로 유지되고 지역변수와 레지스터 변수는 환원
		printf("after longjmp, loc_var = %d, loc_volatile = %d, loc_register = %d\n", loc_var, loc_volatile, loc_register);
		exit(0);
	}
	
	loc_var = 80; //최적화 안하면 메모리, 최적화 하면 register -> 환원(값 변경)
	loc_volatile = 81; //최적화 하든 말든 그대로 유지
	loc_register = 83; //최적화 안하면 메모리, 최적화 하면 register -> 환원(값 변경)
	//변수들에 값 지정해주고 함수 실행
	ssu_nested_func(loc_var, loc_volatile, loc_register);
	exit(0);
}

void ssu_nested_func(int loc_var, int loc_volatile, int loc_register) {
	//longjmp 하기 전에 값 지정해준 변수들 값 출력
	printf("before longjmp, loc_var = %d, lov_volatile = %d, loc_register = %d\n", loc_var, loc_volatile, loc_register);
	longjmp(glob_buffer, 1); //setjmp한 곳으로 돌아감, setjmp는 1을 리턴하게 한다.
}
