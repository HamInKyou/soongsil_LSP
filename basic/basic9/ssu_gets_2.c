#include <stdio.h>
#include <stdlib.h>

#define BUFFER_MAX 256

int main(void)
{
	char command[BUFFER_MAX];
	char *prompt = "Prompt>>"; 

	while (1) {
		fputs(prompt, stdout); //prompt에 저장된 내용 출력
		//표준 입력에 입력한걸 command에 저장
		if (fgets(command, sizeof(command), stdin) == NULL)
			break;
		system(command); //command에 저장된 명령 실행
	}
	
	fprintf(stdout, "Good bye...\n");
	fflush(stdout); //stdout 버퍼를 비워준다.
	exit(0);
}
