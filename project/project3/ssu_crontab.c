#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#define BUFFER_SIZE 1024

int command_separation(char *line, int argc, char (*argv)[BUFFER_SIZE]);
void do_add(int argc, char (*argv)[BUFFER_SIZE]);
void do_remove(int argc, char (*argv)[BUFFER_SIZE]);

int main(void){
	while(1){
		char line[BUFFER_SIZE]; //command 담기 위한 배열
		char argv[7][BUFFER_SIZE] = {0};
		int argc = 0;

		memset(argv, 0, sizeof(argv));
		printf("20162527>");
		fgets(line, sizeof(line), stdin);

		argc = command_separation(line, argc, argv); //command 분할

		if(!strcmp(argv[0], "add")){
			do_add(argc, argv); //명령어 기록 실행
		}
		else if(!strcmp(argv[0], "remove")){
			do_remove(argc, argv); //명령어 삭제 실행
		}
		else if(!strcmp(argv[0], "exit")){
			break;
		}
		else {
			printf("wrong command!\n");
		}

	}
}

int command_separation(char *line, int argc, char (*argv)[BUFFER_SIZE]){
	for(int i = 0; i < (int)strlen(line); i++) //문자 하나씩 검사
	{
		for(int j = 0;; j++, i++) //argv 한칸에 저장
		{
			if(line[i] == ' ' || line[i] == '\n') //space나 개행 나왔을 경우
			{
				if(argc < 6){ //command할 명령어를 입력하기 전일 경우
					argc++; //공백마다 argc 하나씩 늘려줌
					argv[argc][j] = '\0';
					break;
				}
				else{ //command할 명령어를 입력하는 중일 경우
					if(line[i] == '\n'){ //공백은 무시하고 개행만 판단
						argc++;  //전체 command를 마지막 한칸에 저장
						argv[argc][j] = '\0';
						break;
					}
				}
			}

			if(line[i] == '\n')
				break;

			argv[argc][j] = line[i];
		}
	}
	return argc;
}

void do_add(int argc, char (*argv)[BUFFER_SIZE]){
}

void do_remove(int argc, char (*argv)[BUFFER_SIZE]){
}
