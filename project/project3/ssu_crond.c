#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>

#define BUFFER_SIZE 1024
#define MAX_EXPR_SIZE 100
#define MAX_PATH 1024

int command_separation(char *line, int argc, char (*argv)[BUFFER_SIZE]);

int daemon_init(void);
void file_read(); //파일을 읽어서 배열에 담아주는 함수
void charArrTostructArr(); //문자열 배열을 받아서 명령 정보가 담긴 구조체 배열로 바꿔주는 함수
void charTostruct(int argc, char (*argv)[BUFFER_SIZE]); //문자열을 구조체로 바꿔주는 함수

int get_next_token();
int expr();
int term();
int factor();

typedef enum{SLASH, BAR, COMMA, STAR, NUMBER, END} tokens; //토큰 구분하기 위해 만듬
tokens token;
typedef enum{MIN=1, HOUR, DATE, MON, DAYOFWEEK} modes; //실행주기 구분하기 위해 만듬
modes mode;

//명령어 시간주기, 명령어 저장하는 구조체
typedef struct command_info{
	int min[60];
	int hour[24];
	int date[32];
	int month[13];
	int dayofweek[7];
	char command[BUFFER_SIZE];
}CommandInfo;

CommandInfo commandInfo_arr[MAX_EXPR_SIZE]; //명령어 정보들 담기 위한 배열

FILE *fp;
char commandbuf_arr[MAX_EXPR_SIZE][BUFFER_SIZE]; //명령어 담기 위한 배열
char exprbuf[MAX_EXPR_SIZE]; //실행주기 구분하여 임시로 담기 위한 배열
char tokenbuf[10]; //토큰 담기 위한 배열
char pathname_log[MAX_PATH]; //ssu_crontab_log 절대경로 저장하기 위한 배열
char pathname_file[MAX_PATH]; //ssu_crontab_file 절대경로 저장하기 위한 배열
int num;
int j = 0;
int arrSize = 0; //명령어 총 개수
int cur_index = 0; //명령어를 구조체로 바꾸는데, 현재 인덱스

int main(void){
	int i;
	time_t current_time;
	struct tm tm_time;
	
	char timeStr[25] = {'\0',};
	char tmp_pathname[MAX_PATH];
	
	int pid;

	realpath("ssu_crontab_file", pathname_file); //ssu_crontab_file 절대경로 저장
	realpath("ssu_crontab_log", pathname_log); //ssu_crontab_file 절대경로 저장
	getcwd(tmp_pathname, MAX_PATH); //현재 경로 임시 저장

	daemon_init(); //데몬 생성

	chdir(tmp_pathname); //원래 디렉토리로 작업 디렉토리 변경

	while(1){
		current_time = time(NULL); //현재시간 저장
		localtime_r(&current_time, &tm_time); //현재 시간 tm 구조체의 형태로
		strncpy(timeStr, ctime(&current_time), strlen(ctime(&current_time))-1);

		if(tm_time.tm_sec == 0){ //1분마다 실행
			file_read(); //파일 다 읽어서
			charArrTostructArr(); //구조체로 다 바꿔준다.
			
			for(i = 0; i < arrSize; i++){ //구조체 배열에 들어온 명령들 매 분마다 훑어보는데 조건 만족할 경우
				if(commandInfo_arr[i].min[tm_time.tm_min] == 1){
					if(commandInfo_arr[i].hour[tm_time.tm_hour] == 1){
						if(commandInfo_arr[i].date[tm_time.tm_mday] == 1){
							if(commandInfo_arr[i].month[tm_time.tm_mon] == 1){
								if(commandInfo_arr[i].dayofweek[tm_time.tm_wday] == 1){
									system(commandInfo_arr[i].command); //명령어 실행하고
									fp = fopen(pathname_log, "a"); //뒤에 명령어 실행기록 남기기위해 연다.
									fprintf(fp, " [%s] run %s\n", timeStr, commandbuf_arr[i]);//로그를 남긴다.
									fclose(fp);//파일 닫는다.
								}
							}
						}
					}
				}
			}
		}
		sleep(1); //1초마다 현재 시간 받아올 수 있도록 1초씩 쉰다.
	}
}

int daemon_init(void){
   pid_t pid;
   int fd, maxfd;

   //자식 프로세스 생성
   if((pid = fork()) < 0){
      fprintf(stderr, "fork error\n");
      exit(1);
   }
   //규칙1. 백그라운드로 돌리기 위해 부모를 죽인다.
   else if (pid != 0)
      exit(0);

   setsid(); //규칙2. 새로운 프로세스 그룹 생성하고 자신이 그룹 리더가 된다.
   signal(SIGTTIN, SIG_IGN); //규칙3.
   signal(SIGTTOU, SIG_IGN); //작업제어와 연관된 시그널
   signal(SIGTSTP, SIG_IGN); //이 세개를 무시한다.

   //규칙6. 오픈되는 모든 파일 디스크립터를 닫는다.
   maxfd = getdtablesize(); //파일 디스크립터 최대로 생성할 수 있는 크기
   for (fd = 0; fd < maxfd; fd++)
      close(fd);

   //규칙4. 파일모드 생성 마스크를 해제한다.
   umask(0); //디몬이 생성할 파일의 접근허가 모드를 모두 허용한다.
   chdir("/"); //규칙5. 현재 작업 디렉토리를 루트 디렉토리로 설정한다.

   //규칙7. 표준 입출력과 표준 에러를 /dev/null로 재지정한다.
   fd = open ("/dev/null", O_RDWR); //위에서 다 닫았으니 /dev/null이 0번째 디스크립터로
   dup(0); //1번 디스크립터는 0번째 디스크립터를 dup한거
   dup(0); //2번 디스크립터는 0번째 디스크립터를 dup한거

   return 0;
}

int command_separation(char *line, int argc, char (*argv)[BUFFER_SIZE]){
	int j = 0;
	for(int i = 0; i < (int)strlen(line); i++) //문자 하나씩 검사
	{
		if(line[i] == ' '){ //공백이 나오면
			if(argc < 5){ //주기 입력하는 부분일 경우
				argc++; //다음 단위 검사하도록
				j = 0; //다음 단위 첫번째부터 argv에 저장하도록
				continue;
			}
		}
		argv[argc][j] = line[i]; //한글자씩 현재 단위 argv에 저장
		j++;
	}
	argc++; //반복문 끝났을 경우 argc는 명령부분은 체크 안했기 때문에 +1해준다.
	return argc;
}

//파일을 읽어서 배열에 담아주는 함수
void file_read(){
	char buf[BUFFER_SIZE];
	int i = 0;

	fp = fopen(pathname_file, "r+");

	fseek(fp, 0, SEEK_SET);
	while (fgets(buf, BUFFER_SIZE, fp) != NULL){
		memset(commandbuf_arr[i], 0, BUFFER_SIZE); //담기 전에 초기화
		strncpy(commandbuf_arr[i], buf, strlen(buf)-1); //배열에 개행 빼고 복사
		i++;
	}
	arrSize = i; //array에 몇개 명령 들어갔는지

	fclose(fp);
}

void charArrTostructArr(){ //문자열 배열을 받아서 명령 정보가 담긴 구조체 배열로 바꿔주는 함수
	int i, k;

	char argv[6][BUFFER_SIZE] = {0};
	int argc = 0;

	for(i = 0; i < arrSize; i++){
		argc = 0;
		memset(argv, 0, sizeof(argv));

		argc = command_separation(commandbuf_arr[i], argc, argv); //한줄의 명령어를 분할, argc, argv 저장해줌
		charTostruct(argc, argv);
	}
}

void charTostruct(int argc, char (*argv)[BUFFER_SIZE]){ //문자열을 구조체로 바꿔주는 함수
	int i;

	for(i = 0; i < 5; i++){ //실행주기 차례대로 검사
		j = 0;

		strcpy(exprbuf, argv[i]); //현재 검사할 실행주기 exprbuf에 복사
		mode = i+1;

		get_next_token(); //맨 처음 토큰 읽고
		expr(); //식 검사
		strcpy(commandInfo_arr[cur_index].command, argv[5]);

	}
	cur_index++; 
}

//토큰 얻는 함수
int get_next_token(){
	int i = 0;
	tokenbuf[i++] = exprbuf[j++]; //시간주기에 있는걸 한글자씩 토큰으로 옮겨옴
	if(tokenbuf[0] == '/'){ //토큰 SLASH 구분
		token = SLASH;
	}
	else if(tokenbuf[0] == '-'){ //토큰 BAR 구분
		token = BAR;
	}
	else if(tokenbuf[0] == ','){ //토큰 COMMA 구분
		token = COMMA;
	}
	else if(tokenbuf[0] == '*'){ //토큰 STAR 구분
		token = STAR;
	}
	else if(tokenbuf[0] >= '0' && tokenbuf[0] <= '9'){ //토큰 NUMBER 구분
		while(exprbuf[i] >= '0' && exprbuf[i] <= '9'){
			tokenbuf[i++] = exprbuf[j++];
		}
		num = atoi(tokenbuf);

		//현재 검사중인 시간 주기에 따라 숫자 범위 넘어가면 그만하고 에러 출력하게
		if(mode == MIN){
			if(num < 0 || num > 59)
				return -1;
		}
		else if(mode == HOUR){
			if(num < 0 || num > 23)
				return -1;
		}
		else if(mode == DATE){
			if(num < 1 || num > 31)
				return -1;
		}
		else if(mode == MON){
			if(num < 1 || num > 12)
				return -1;
		}
		else if(mode == DAYOFWEEK){
			if(num < 0 || num > 6)
				return -1;
		}
		token = NUMBER;
	}
	else if(tokenbuf[0] == 0){ //배열의 끝일 시에 토큰 END로
		token = END;
	}
	else{ //딴거 이상한거 받으면 그만하고 에러 출력하게
		return -1;
	}
	return 1;
}

//토큰 ','로 구분하는 우선순위 제일 낮은 함수
int expr(){
	int isOk;
	if((isOk = term()) == -1) //term 받는다.
		return -1;
	while(token == COMMA){
		if(get_next_token() == -1) //다음 토큰을 받는다.
			return -1;
		if(term() == -1) //그 다음 토큰으로 term 받는다.
			return -1;
	}	
	return 1;
}

//토큰 '/', '-'로 구분하는 우선순위 그 다음 높은 함수
int term(){
	int num1, num2, num3;
	int range;
	int k,l;

	num1 = factor();

	if(num1 == -1){ //팩터 이상한거 골랐을 경우
		return -1; 
	}

	else if(num1 == -2){ //팩터가 *일 경우
		if(token == BAR) //BAR는 * 다음에 나오면 안되니까 맞지 않음!
			return -1; 
		else if(token == SLASH){ //SLASH가 나왔을 경우
			if(get_next_token() == -1) 
				return -1; 
			//다음 토큰으로 이상한거나 별 나왔을 경우 (별은 SLASH다음 올 수 없음)
			if((num2 = factor())==-1 || num2 == -2) 
				return -1; 

			if(mode == MIN){ //현재 분 검사중일 경우
				l = 1;
				for(k = 0; k < 60; k++, l++){
					if(l%num2 == 0)
						commandInfo_arr[cur_index].min[k] = 1;
				}
			}
			else if(mode == HOUR){ //현재 시 검사중일 경우
				l = 1;
				for(k = 0; k < 24; k++, l++){ // '/'뒤에 나온 숫자만큼 건너뛰며 켜준다.
					if(l%num2 == 0)
						commandInfo_arr[cur_index].hour[k] = 1;
				}
			}
			else if(mode == DATE){ //현재 일 검사중일 경우
				l = 1;
				for(k = 1; k < 32; k++, l++){ // '/'뒤에 나온 숫자만큼 건너뛰며 켜준다.
					if(l%num2 == 0)
						commandInfo_arr[cur_index].date[k] = 1;
				}
			}
			else if(mode == MON){ //현재 월 검사중일 경우
				l = 1;
				for(k = 1; k < 13; k++, l++){ // '/'뒤에 나온 숫자만큼 건너뛰며 켜준다.
					if(l%num2 == 0)
						commandInfo_arr[cur_index].month[k] = 1;
				}
			}
			else if(mode == DAYOFWEEK){ //현재 요일 검사중일 경우
				l = 1;
				for(k = 0; k < 7; k++, l++){ // '/'뒤에 나온 숫자만큼 건너뛰며 켜준다.
					if(l%num2 == 0)
						commandInfo_arr[cur_index].dayofweek[k] = 1;
				}
			}

		}

		else if(token == END){ //*밖에 없을 경우
			if(mode == MIN){ //현재 분 검사중일 경우
				for(k = 0; k < 60; k++){ //모든 분 켜준다.
					commandInfo_arr[cur_index].min[k] = 1;
				}
			}
			else if(mode == HOUR){ //현재 시 검사중일 경우
				for(k = 0; k < 24; k++){ //모든 시 켜준다.
					commandInfo_arr[cur_index].hour[k] = 1;
				}
			}
			else if(mode == DATE){ //현재 일 검사중일 경우
				for(k = 1; k < 32; k++){ //모든 일 켜준다.
					commandInfo_arr[cur_index].date[k] = 1;
				}
			}
			else if(mode == MON){ //현재 월 검사중일 경우
				for(k = 1; k < 13; k++){ //모든 월 켜준다.
					commandInfo_arr[cur_index].month[k] = 1;
				}
			}
			else if(mode == DAYOFWEEK){ //현재 요일 검사중일 경우
				for(k = 0; k < 7; k++){ //모든 요일 켜준다.
					commandInfo_arr[cur_index].dayofweek[k] = 1;
				}
			}
		}
	} 

	else{ //팩터가 숫자일 경우
		if(token == BAR){ //뒤에 BAR 왔을 경우
			if(get_next_token() == -1) 
				return -1; 
			//다음 토큰으로 이상한거나 별 나왔을 경우 (별은 BAR다음 올 수 없음)
			if((num2 = factor()) == -1 || num2 == -2) 
				return -1; 
			if(num1 > num2) // BAR 앞에게 뒤에거보다 클 경우
				return -1;
			if(token == SLASH){ // num1-num2/ 형태로 나왔을 경우
				range = num2 - num1 + 1; //num3 쓸 수 있는 범위
				if(get_next_token() == -1) 
					return -1; 
				//다음 토큰으로 이상한거나 별 나왔을 경우 (별은 SLASH다음 올 수 없음)
				if((num3 = factor()) == -1 || num3 == -2) 
					return -1; 
				if(num3 < 0 || num3 > range)
					return -1;
				l = 1;
				for(k = num1; k <= num2; k++, l++){
					if(l%num3 == 0){
						if(mode == MIN)
							commandInfo_arr[cur_index].min[k] = 1;
						else if(mode == HOUR)
							commandInfo_arr[cur_index].hour[k] = 1;
						else if(mode == DATE)
							commandInfo_arr[cur_index].date[k] = 1;
						else if(mode == MON)
							commandInfo_arr[cur_index].month[k] = 1;
						else if(mode == DAYOFWEEK)
							commandInfo_arr[cur_index].dayofweek[k] = 1;
					}
				}
			}
			else{ //num1-num2의 형태가 끝일 경우
				for(k = num1; k <= num2; k++){
					if(mode == MIN)
						commandInfo_arr[cur_index].min[k] = 1;
					else if(mode == HOUR)
						commandInfo_arr[cur_index].hour[k] = 1;
					else if(mode == DATE)
						commandInfo_arr[cur_index].date[k] = 1;
					else if(mode == MON)
						commandInfo_arr[cur_index].month[k] = 1;
					else if(mode == DAYOFWEEK)
						commandInfo_arr[cur_index].dayofweek[k] = 1;
				}
			}
		}
		else if(token == SLASH) //숫자 바로 뒤에 SLASH가 오지 못함
			return -1;
		else if(token == COMMA || token == END){ //뒤에 COMMA 왔거나 끝일 경우
			if(mode == MIN)
				commandInfo_arr[cur_index].min[num1] = 1;
			else if(mode == HOUR)
				commandInfo_arr[cur_index].hour[num1] = 1;
			else if(mode == DATE)
				commandInfo_arr[cur_index].date[num1] = 1;
			else if(mode == MON)
				commandInfo_arr[cur_index].month[num1] = 1;
			else if(mode == DAYOFWEEK)
				commandInfo_arr[cur_index].dayofweek[num1] = 1;
		}

	}
	return 1;
}

//숫자 혹은 *을 받기 위한 우선순위 제일 높은 함수
int factor(){
	int r;

	//숫자일 경우 숫자 리턴
	if(token == NUMBER) { 
		if(get_next_token() == -1)
			return -1;
		r = num;
		return r;
	}
	//STAR일 경우 -2 리턴
	else if(token == STAR) {
		if(get_next_token() == -1)
			return -1;
		return -2;
	}
	//둘 중 아무것도 아닐 경우 -1 리턴
	else{
		return -1;
	}
}

