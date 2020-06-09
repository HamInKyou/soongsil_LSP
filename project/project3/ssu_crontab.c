#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <time.h>
#include <sys/time.h>

#define BUFFER_SIZE 1024
#define MAX_EXPR_SIZE 100

int command_separation(char *line, int argc, char (*argv)[BUFFER_SIZE]);
void file_read(); //파일을 읽어서 배열에 담아주는 함수
int get_next_token();
int expr();
int term();
int factor();
int do_add(int argc, char (*argv)[BUFFER_SIZE]);
int do_remove(int argc, char (*argv)[BUFFER_SIZE]);

typedef enum{SLASH, BAR, COMMA, STAR, NUMBER, END} tokens; //토큰 구분하기 위해 만듬
tokens token;
typedef enum{MIN=1, HOUR, DATE, MON, DAYOFWEEK} modes; //실행주기 구분하기 위해 만듬
modes mode;

FILE *fp;
char *fname = "ssu_crontab_file";
char commandbuf_arr[MAX_EXPR_SIZE][BUFFER_SIZE]; //명령어 담기 위한 배열
char exprbuf[MAX_EXPR_SIZE]; //실행주기 구분하여 임시로 담기 위한 배열
char tokenbuf[10]; //토큰 담기 위한 배열
int num;
int j = 0;
int arrSize = 0;

int main(void){
	struct timeval startTime, endTime;
	double diffTime;

	int i;
	
	gettimeofday(&startTime, NULL);

	fp = fopen(fname, "a");
	fclose(fp);

	while(1){
		char line[BUFFER_SIZE]; //command 담기 위한 배열
		char argv[7][BUFFER_SIZE] = {0};
		int argc = 0;
		
		file_read();
		for(i = 0; i < arrSize; i++){
			printf("%d. %s\n", i, commandbuf_arr[i]);
		}
		printf("\n");

		memset(argv, 0, sizeof(argv));
		printf("20162527>");
		fgets(line, sizeof(line), stdin);

		argc = command_separation(line, argc, argv); //command 분할

		if(!strcmp(argv[0], "add")){
			if(do_add(argc, argv) == -1) //명령어 기록 실행
				printf("wrong command!\n");

		}
		else if(!strcmp(argv[0], "remove")){
			if(do_remove(argc, argv) == -1)
				printf("wrong command!\n");
		}
		else if(!strcmp(argv[0], "exit")){
			break;
		}
		else {
			if(argv[0][0] == '\0')
				continue;
			else
				printf("wrong command!\n");
		}

	}
	fclose(fp);

	gettimeofday(&endTime, NULL);
	diffTime = (endTime.tv_sec - startTime.tv_sec) + ((endTime.tv_usec - startTime.tv_usec)/1000000);
	printf("소요시간 : %f sec\n", diffTime);
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

//파일을 읽어서 배열에 담아주는 함수
void file_read(){
	char buf[BUFFER_SIZE];
	int i = 0;
	
	fp = fopen(fname, "r+");
	
	fseek(fp, 0, SEEK_SET);
	while (fgets(buf, BUFFER_SIZE, fp) != NULL){
		memset(commandbuf_arr[i], 0, BUFFER_SIZE); //담기 전에 초기화
		strncpy(commandbuf_arr[i], buf, strlen(buf)-1); //배열에 개행 빼고 복사
		i++;
	}
	arrSize = i; //array에 몇개 명령 들어갔는지
	
	fclose(fp);
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
		while(exprbuf[j] >= '0' && exprbuf[j] <= '9'){
			tokenbuf[i++] = exprbuf[j++];
		}
		tokenbuf[i] = '\0';
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
	if((isOk = term()) == -1) //factor 받는다.
		return -1;
	while(token == COMMA){
		if(get_next_token() == -1) //다음 토큰을 받는다.
			return -1;
		if(term() == -1) //그 다음 토큰으로 factor 받는다.
			return -1;
	}	
	return 1;
}

//토큰 '/', '-'로 구분하는 우선순위 그 다음 높은 함수
int term(){
   int num1, num2, num3;
   int range;
   num1 = factor();
   if(num1 == -1) //팩터 이상한거 골랐을 경우
      return -1; 
   else if(num1 == -2){ //팩터가 *일 경우
      if(token == BAR) //BAR는 * 다음에 나오면 안되니까 맞지 않음!
         return -1; 
      else if(token == SLASH){ //SLASH가 나왔을 경우
         if(get_next_token() == -1) 
            return -1; 
         //다음 토큰으로 이상한거나 별 나왔을 경우 (별은 SLASH다음 올 수 없음)
         if((num2 = factor())==-1 || num2 == -2) 
            return -1; 
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
         }
      }
      else if(token == SLASH) //숫자 바로 뒤에 SLASH가 오지 못함
         return -1;
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

//명령어 추가하는 add함수
int do_add(int argc, char (*argv)[BUFFER_SIZE]){
	time_t currentTime;
	char timeStr[25] = {'\0',};
	int i;
	int isOk1, isOk2; 

	for(i = 1; i <= 5; i++){ //실행 주기 차례대로 검사
		j = 0;

		strcpy(exprbuf, argv[i]); //현재 검사할 실행주기 exprbuf에 복사
		mode = i; //현재 어떤 실행주기를 검사중인지 표시
		
		isOk1 = get_next_token(); //맨 처음 토큰을 읽는다.
		isOk2 = expr(); //식을 검사한다.
		
		if(isOk1 == -1 || isOk2 == -1){ //문제가 있다면 에러출력하고 프롬포트로 돌아가게
			return -1;
		}
	}

	//반복문 무사히 마쳤다는 것은 실행주기에 문제가 없다는 것.
	fp = fopen(fname, "r+");
	fseek(fp, 0, SEEK_END); //맨 끝으로 이동
	fprintf(fp, "%s %s %s %s %s %s\n", argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]); //명령문 추가
	fclose(fp);
	
	time(&currentTime);
	strncpy(timeStr, ctime(&currentTime), strlen(ctime(&currentTime))-1);
	fp = fopen("ssu_crontab_log", "a");
	fprintf(fp, " [%s] add %s %s %s %s %s %s\n", timeStr, argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
	fclose(fp);

	return 1;
}

int do_remove(int argc, char (*argv)[BUFFER_SIZE]){
	time_t currentTime;
	char timeStr[25] = {'\0',};
	char wantDeleteStr[BUFFER_SIZE] = {'\0',};
	int wantDeleteIndex = 0;
	int fd;
	int i;
	
	if((wantDeleteIndex = atoi(argv[1])) >= arrSize) //입력한게 범위 벗어났을 경우
		return -1;
	if(arrSize == 1){ //한개밖에 없을 경우
		strcpy(wantDeleteStr, commandbuf_arr[wantDeleteIndex]);
	}
	else if(wantDeleteIndex == arrSize){ //삭제하고 싶은게 맨 끝에거일 경우
		strcpy(wantDeleteStr, commandbuf_arr[wantDeleteIndex]);
	}
	for(i = wantDeleteIndex; i < arrSize - 1; i++){
		strcpy(wantDeleteStr, commandbuf_arr[i]); //지울 내용 임시 저장
		memset(commandbuf_arr[i], 0, BUFFER_SIZE); //담기 전에 내용 비우기
		strcpy(commandbuf_arr[i], commandbuf_arr[i+1]); //그 다음거를 땡겨온다.
	}
	memset(commandbuf_arr[arrSize-1], 0, BUFFER_SIZE); //땡겨와서 필요없어진 끝에거 비우기
	arrSize -= 1; //arrSize 하나 줄인다.
	
	fp = fopen(fname, "r+");

	fd = fileno(fp);
	ftruncate(fd, 0); //파일의 내용 다 지워주고 사이즈를 0으로 해준다.
	fseek(fp, 0, SEEK_SET); //파일 포인터 위치 파일 시작점으로 보내준다.
	
	//다 비운 파일에 바뀐 내용 다시 써준다.
	for(i = 0; i < arrSize; i++){
		fprintf(fp, "%s\n", commandbuf_arr[i]);
	}

	fclose(fp);
	
	time(&currentTime);
	strncpy(timeStr, ctime(&currentTime), strlen(ctime(&currentTime))-1);
	fp = fopen("ssu_crontab_log", "a");
	fprintf(fp, " [%s] remove %s\n", timeStr, wantDeleteStr);
	fclose(fp);

	return 1;
}
