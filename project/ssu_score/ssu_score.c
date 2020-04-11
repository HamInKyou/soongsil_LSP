#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "ssu_score.h"
#include "blank.h"

extern struct ssu_scoreTable score_table[QNUM];
extern char id_table[SNUM][10];

struct ssu_scoreTable score_table[QNUM];
char id_table[SNUM][10];

char stuDir[BUFLEN];
char ansDir[BUFLEN];
char errorDir[BUFLEN];
char threadFiles[ARGNUM][FILELEN];
char cIDs[ARGNUM][FILELEN];

int eOption = false;
int tOption = false;
int cOption = false;

void ssu_score(int argc, char *argv[])
{
	char saved_path[BUFLEN]; //1024바이트짜리 배열생성
	int i;

	for(i = 0; i < argc; i++){//인자 검사하는데 -h 옵션 발견했을경우
		if(!strcmp(argv[i], "-h")){
			print_usage(); //사용방법 출력
			return;
		}
	}

	memset(saved_path, 0, BUFLEN); //배열을 0으로 다 채워준다.
	if(argc >= 3 && strcmp(argv[1], "-c") != 0){//첫번째 인자로 들어온 옵션이 -c가 아니고 인자가 2개 이상일 경우
		strcpy(stuDir, argv[1]); //stuDir에 첫번째 인자 복사
		strcpy(ansDir, argv[2]); //ansDir에 두번째 인자 복사
	}

	if(!check_option(argc, argv)) //옵션 체크
		exit(1); //설정하지 않은 옵션이 인자로 찍혔을 경우 끝내기.

	if(!eOption && !tOption && cOption){ //c옵션만 켜져있을 경우
		do_cOption(cIDs); //cIDs에 저장한 인자들로 cOption에 대한 함수 실행
		return;
	}

	getcwd(saved_path, BUFLEN); //saved_path에 현재 작업 디렉토리의 full path명을 저장

	if(chdir(stuDir) < 0){ //현재 작업 디렉토리를 stuDir에 저장된 디렉토리로 변경
		fprintf(stderr, "%s doesn't exist\n", stuDir);
		return;
	}
	getcwd(stuDir, BUFLEN); //stuDir에 현재 작업 디렉토리의 full path명을 저장

	chdir(saved_path); //현재 작업 디렉토리를 saved_path에 저장했던 경로로 변경
	if(chdir(ansDir) < 0){ //현재 작업 디렉토리를 ansDir에 저장된 디렉토리로 변경
		fprintf(stderr, "%s doesn't exist\n", ansDir);
		return;
	}
	getcwd(ansDir, BUFLEN); //ansDir에 현재 작업 디렉토리의 full path명을 저장

	chdir(saved_path); //현재 작업 디렉토리를 saved_path에 저장했던 경로로 변경

	set_scoreTable(ansDir); //문제의 점수와 관련된 scoreTable 설정하는 함수 호출
	set_idTable(stuDir); //학생들의 정보와 관련된 idTable 설정하는 함수 호출

	printf("grading student's test papers..\n");
	score_students(); //점수를 매기는 함수 호출

	if(cOption) //c옵션이 켜져있을 경우
		do_cOption(cIDs); //c옵션 관련 함수 실행

	return;
}

//옵션 체크하는 함수
int check_option(int argc, char *argv[])
{
	int i, j;
	int c;
	
	//명령행의 옵션을 받는데, e옵션 뒤에는 인자를 제공하고 
	//t,h,p,c옵션 뒤에는 인자를 제공하지 않는다.
	while((c = getopt(argc, argv, "e:thc")) != -1)
	{
		switch(c){
			case 'e': //e일 경우
				eOption = true; //e옵션을 키고
				strcpy(errorDir, optarg); //-e 뒤에 오는 인자를 errorDir에 복사

				if(access(errorDir, F_OK) < 0) //errorDir에 해당하는 폴더가 존재하지 않을경우
					mkdir(errorDir, 0755); //해당 폴더 생성(_rwxr_xr_x 권한으로)
				else{ //존재할 경우
					rmdirs(errorDir); //해당 폴더 지우고 
					mkdir(errorDir, 0755); //해당 폴더 재생성(_rwxr_xr_x 권한으로)
				}
				break;
			case 't': //t일 경우
				tOption = true; //t옵션을 키고
				i = optind; //인수로 받은 문자열 중에 처리하지 못한 인수에 대한 인덱스 번호 저장
				j = 0;

				while(i < argc && argv[i][0] != '-'){ //인자를 차례대로 체크, 옵션이 아닌애들 중에서

					if(j >= ARGNUM) //정해둔 인자 수(5)를 넘어섰을 경우
						printf("Maximum Number of Argument Exceeded.  :: %s\n", argv[i]);
					else 
						strcpy(threadFiles[j], argv[i]); //threadFiles에 인자 추가
					i++; 
					j++;
				}
				break;
			case 'c': //c일 경우
				cOption = true; //c옵션을 킨다.
				i = optind; //인수로 받은 문자열 중에 처리하지 못한 인수에 대한 인덱스 번호 저장
				j = 0;

				while(i < argc && argv[i][0] != '-'){ //인자를 차례대로 체크, 옵션이 아닌애들 중에서

					if(j >= ARGNUM) //정해둔 인자 수(5)를 넘어섰을 경우
						printf("Maximum Number of Argument Exceeded.  :: %s\n", argv[i]);
					else 
						strcpy(cIDs[j], argv[i]); //cID에 인자 추가
					i++; 
					j++;
				}
				break;
			case '?': //설정하지 않은 옵션이 인자로 쓰였을 경우
				printf("Unkown option %c\n", optopt);
				return false;
		}
	}

	return true;
}


void do_cOption(char (*ids)[FILELEN])
{
	FILE *fp; 
	char tmp[BUFLEN];
	int i = 0;
	char *p, *saved;

	if((fp = fopen("score.csv", "r")) == NULL){ //읽기모드로 score.csv 열기
		fprintf(stderr, "file open error for score.csv\n");
		return;
	}

	fscanf(fp, "%s\n", tmp); //파일에서 읽어서 tmp에 저장한다.

	while(fscanf(fp, "%s\n", tmp) != EOF) //파일의 끝에 도달할 때까지 읽는다.
	{
		p = strtok(tmp, ","); //받은 문자열 ',' 기준으로 자른다. 자르고 맨 앞에거 저장

		if(!is_exist(ids, tmp)) //파일의 현재 id가 찾으려는 ids에 존재하는지?
			continue; //없으면 다음거로

		printf("%s's score : ", tmp); 

		while((p = strtok(NULL, ",")) != NULL) //','로 끊은거의 다음 차례거로 넘어감
			saved = p; //saved에 위에서 넘어간 수치(점수) 저장

		printf("%s\n", saved); //점수 출력
	}
	fclose(fp); //파일 닫기
}

int is_exist(char (*src)[FILELEN], char *target)
{
	int i = 0;

	while(1)
	{
		if(i >= ARGNUM) //id배열 크기(5) 넘어갔을 경우 false 리턴
			return false;
		else if(!strcmp(src[i], "")) //비어있으면 false 리턴
			return false;
		else if(!strcmp(src[i++], target)) //찾는게 있느면 true 리턴
			return true;
	}
	return false;
}

void set_scoreTable(char *ansDir)
{
	char filename[FILELEN]; 

	sprintf(filename, "%s/%s", ansDir, "score_table.csv");//filename 저장. ansDir/score_table.csv

	if(access(filename, F_OK) == 0) //테이블 파일이 존재할 경우
		read_scoreTable(filename); //테이블  파일 읽는 함수 호출
	else{ //존재하지 않을 경우
		make_scoreTable(ansDir); //테이블 만드는 함수 호출
		write_scoreTable(filename); //테이블 파일에 쓰는 함수 호출
	}
}

void read_scoreTable(char *path) 
{
	FILE *fp; 
	char qname[FILELEN]; //퀴즈 이름담는 배열
	char score[BUFLEN]; //점수 담는 배열
	int idx = 0;

	if((fp = fopen(path, "r")) == NULL){ //읽기모드로 파일 연다(테이블)
		fprintf(stderr, "file open error for %s\n", path);
		return ;
	}
	
	//파일에서 읽는데, ','전까지 읽어서 qname에, 뒷부분을 score에 저정한다.
	while(fscanf(fp, "%[^,],%s\n", qname, score) != EOF){
		strcpy(score_table[idx].qname, qname); //score_table의 qname에 qname을 저장하고
		score_table[idx++].score = atof(score); //score_table의 score에 socre을 실수로 변환하여 저장.
	}

	fclose(fp); //열었던 파일 닫는다.
}

void make_scoreTable(char *ansDir)
{
	int type, num;
	double score, bscore, pscore;
	struct dirent *dirp;
	DIR *dp;
	char tmp[BUFLEN];
	int idx = 0;
	int i;

	num = get_create_type(); //점수 어떻게 입력할지 사용자한테 물어보고 답받는다.

	if(num == 1) //빈칸 문제와 프로그램 문제에 따로 점수 입력하는 경우
	{
		printf("Input value of blank question : ");
		scanf("%lf", &bscore); //빈칸 문제의 점수를 담는 bscore에 점수 저장
		printf("Input value of program question : ");
		scanf("%lf", &pscore); //프로그램 문제의 점수를 담는 psocre에 점수 저장
	}
	
	if((dp = opendir(ansDir)) == NULL){ //ansDir 연다.
		fprintf(stderr, "open dir error for %s\n", ansDir);
		return;
	}	

	while((dirp = readdir(dp)) != NULL) //ansDir 안에 있는 파일을 하나씩 읽는다.
	{
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, "..")) //".",".."은 뛰어넘기
			continue;

		if((type = get_file_type(dirp->d_name)) < 0) //해당 파일이 .txt 파일인지 .c 파일인지 검사
			continue;
		
		strcpy(score_table[idx++].qname, dirp->d_name); //해당 파일 이름을 score_table에 저장하고 다음 인덱스로 
	}

	closedir(dp); //ansDir 닫는다.
	sort_scoreTable(idx); //scoreTable 문제 번호순으로 정렬해준다.

	for(i = 0; i < idx; i++)
	{
		type = get_file_type(score_table[i].qname); //해당 파일이 .txt 파일인지 .c 파일인지 검사

		if(num == 1) //빈칸 문제와 프로그램 문제에 따로 점수 입력하는 경우
		{
			if(type == TEXTFILE)
				score = bscore; //빈칸 문제에 점수배정
			else if(type == CFILE)
				score = pscore; //프로그램 문제에 점수 배정
		}
		else if(num == 2) //문제 각각에 대하여 점수 배정하는 경우 해당 문제의 점수를 입력받아 저장
		{
			printf("Input of %s: ", score_table[i].qname); 
			scanf("%lf", &score);
		}

		score_table[i].score = score; //그 문제의 점수를 정해준다.
	}
}

void write_scoreTable(char *filename)
{
	int fd;
	char tmp[BUFLEN];
	int i;
	int num = sizeof(score_table) / sizeof(score_table[0]); //테이블 전체 행 개수

	if((fd = creat(filename, 0666)) < 0){ //_rw_rw_rw권한으로 파일을 생성한다.
		fprintf(stderr, "creat error for %s\n", filename);
		return;
	}

	for(i = 0; i < num; i++) //테이블 전체 반복
	{
		if(score_table[i].score == 0) //score 0이면 그만
			break;
		
		//tmp에 문제이름,점수 형식으로 저장
		sprintf(tmp, "%s,%.2f\n", score_table[i].qname, score_table[i].score);
		write(fd, tmp, strlen(tmp)); //creat한 파일에 tmp를 차례대로 쭉 써준다.
	}

	close(fd); //파일 닫는다.
}


void set_idTable(char *stuDir)
{
	struct stat statbuf; //
	struct dirent *dirp; //디렉토리 안의 파일을 가리키는 구조체 포인터
	DIR *dp; //디렉토리 가리키는 포인터
	char tmp[BUFLEN];
	int num = 0;

	if((dp = opendir(stuDir)) == NULL){ //stuDir 디렉토리를 연다.
		fprintf(stderr, "opendir error for %s\n", stuDir);
		exit(1);
	}

	while((dirp = readdir(dp)) != NULL){ //stuDir 내의 파일을 차례로 읽는다.
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, "..")) //'.'과'..'을 읽고나서
			continue;

		sprintf(tmp, "%s/%s", stuDir, dirp->d_name); // 디렉토리/디렉토리 내의 파일 경로를 tmp에 저장한다.
		stat(tmp, &statbuf); //tmp에 저장된 파일 이름의 정보를 statbuf에 저장한다.

		if(S_ISDIR(statbuf.st_mode)) //그 파일이 디렉토리파일일 경우
			strcpy(id_table[num++], dirp->d_name); //id_table의 한 행에 그 디렉토리파일의 이름을 저장한다.
		else
			continue;
	}

	sort_idTable(num); //그 디렉토리들을 이름순으로 정렬하는 함수를 호출한다.
}

void sort_idTable(int size) //이름순으로 정렬해주는 함수
{
	int i, j;
	char tmp[10]; //임시 배열

	for(i = 0; i < size - 1; i++){ //이름순으로 정렬해주는 반복과정
		for(j = 0; j < size - 1 -i; j++){
			if(strcmp(id_table[j], id_table[j+1]) > 0){ //제일 큰거를 뒤로 보내주면서 정렬한다.
				strcpy(tmp, id_table[j]);
				strcpy(id_table[j], id_table[j+1]);
				strcpy(id_table[j+1], tmp);
			}
		}
	}
}

void sort_scoreTable(int size) //scoreTable 문제 번호순으로 정렬해주는 함수
{
	int i, j;
	struct ssu_scoreTable tmp;
	int num1_1, num1_2;
	int num2_1, num2_2;

	for(i = 0; i < size - 1; i++){ //처음부터 테이블 끝까지 검사
		for(j = 0; j < size - 1 - i; j++){ 

			get_qname_number(score_table[j].qname, &num1_1, &num1_2); //현재 파일의 이름 알아오기
			get_qname_number(score_table[j+1].qname, &num2_1, &num2_2); //다음 파일의 이름 알아오기


			if((num1_1 > num2_1) || ((num1_1 == num2_1) && (num1_2 > num2_2))){ //문제 번호 현재거가 더 크다면
				//아래 세줄은 큰거를 뒤로 보내는 과정
				memcpy(&tmp, &score_table[j], sizeof(score_table[0])); 
				memcpy(&score_table[j], &score_table[j+1], sizeof(score_table[0]));
				memcpy(&score_table[j+1], &tmp, sizeof(score_table[0]));
			}
		}
	}
}

void get_qname_number(char *qname, int *num1, int *num2)
{
	char *p;
	char dup[FILELEN];
	

	strncpy(dup, qname, strlen(qname));//qname의 문자열을 dup에 qname의 사이즈만큼 복사
	*num1 = atoi(strtok(dup, "-.")); //'.'과 '-'로 구분하여 dup을 분리한다. 제일 첫번째로 나온거 정수로 바꿔주고 저장
	
	p = strtok(NULL, "-.");//그 다음으로 나온거 포인터가 가리키게
	if(p == NULL) //나온게 없다면
		*num2 = 0; //num2는 0
	else //나왔다면
		*num2 = atoi(p); //정수로 변환하여 num2에 저장 
}

//score_table 존재하지 않을 때 빈칸문제와 프로그램 문제에 점수 넣는 방식 입력받기
int get_create_type()
{
	int num;

	while(1)
	{
		printf("score_table.csv file doesn't exist in TREUDIR!\n");
		printf("1. input blank question and program question's score. ex) 0.5 1\n");
		printf("2. input all question's score. ex) Input value of 1-1: 0.1\n");
		printf("select type >> ");
		scanf("%d", &num); //입력받아서

		if(num != 1 && num != 2) //해당 안되면 다시 입력받게
			printf("not correct number!\n");
		else
			break;
	}

	return num;
}

void score_students() 
{
	double score = 0;
	int num;
	int fd;
	char tmp[BUFLEN]; //임시 배열
	int size = sizeof(id_table) / sizeof(id_table[0]); //테이블의 행 개수 저장

	if((fd = creat("score.csv", 0666)) < 0){ //score.csv를 생성한다.
		fprintf(stderr, "creat error for score.csv");
		return;
	}
	write_first_row(fd); //첫번째 열을 채워주는 함수 호출

	for(num = 0; num < size; num++) //처음부터 끝까지 모든 행을 검사하며
	{
		if(!strcmp(id_table[num], "")) //해당 행이 비어있을 경우 그만
			break;

		sprintf(tmp, "%s,", id_table[num]); //tmp에 id_table의 현재 행을 "아이디," 꼴로 저장해준다.
		write(fd, tmp, strlen(tmp));  //tmp에 저장한걸 파일에 써준다.

		score += score_student(fd, id_table[num]); 
	}

	printf("Total average : %.2f\n", score / num);

	close(fd);
}

double score_student(int fd, char *id)
{
	int type;
	double result;
	double score = 0;
	int i;
	char tmp[BUFLEN];
	int size = sizeof(score_table) / sizeof(score_table[0]); //score_table의 전체 행 개수 저장

	for(i = 0; i < size ; i++) //전체 행 반복
	{
		if(score_table[i].score == 0) //해당 행의 score가 0이면 그만
			break;

		sprintf(tmp, "%s/%s/%s", stuDir, id, score_table[i].qname); //tmp에 디렉토리/아이디/문제번호 저장

		if(access(tmp, F_OK) < 0) //그러한 파일이 존재하는지?
			result = false;
		else //존재한다면
		{
			if((type = get_file_type(score_table[i].qname)) < 0) //.txt 파일인지 .c 파일인지 알아내서
				continue;
			
			if(type == TEXTFILE) //textfile일 경우
				result = score_blank(id, score_table[i].qname); 
			else if(type == CFILE) //.c 파일일 경우
				result = score_program(id, score_table[i].qname);
		}

		if(result == false)
			write(fd, "0,", 2);
		else{
			if(result == true){
				score += score_table[i].score;
				sprintf(tmp, "%.2f,", score_table[i].score);
			}
			else if(result < 0){
				score = score + score_table[i].score + result;
				sprintf(tmp, "%.2f,", score_table[i].score + result);
			}
			write(fd, tmp, strlen(tmp));
		}
	}

	printf("%s is finished.. score : %.2f\n", id, score); 

	sprintf(tmp, "%.2f\n", score);
	write(fd, tmp, strlen(tmp));

	return score;
}

void write_first_row(int fd)
{
	int i;
	char tmp[BUFLEN];
	int size = sizeof(score_table) / sizeof(score_table[0]); //score_table의 전체 행 개수 저장

	write(fd, ",", 1); //인자로 불러온 fd가 가리키는 파일에 ','을 입력한다.

	for(i = 0; i < size; i++){ //행을 하나씩 검사하면서
		if(score_table[i].score == 0) //점수가 0일경우 그만
			break;
		
		sprintf(tmp, "%s,", score_table[i].qname); //tmp에 "퀴즈이름,"을 저장한다.
		write(fd, tmp, strlen(tmp));//fd가 가리키는 파일에 "퀴즈이름,"을 써준다.
	}
	write(fd, "sum\n", 4); //반복문 다 끝나고 뒤에 "sum\n"을 붙인다.
}

char *get_answer(int fd, char *result)
{
	char c;
	int idx = 0;

	memset(result, 0, BUFLEN); //result에 인자로 들어온 배열을 초기화한다.
	while(read(fd, &c, 1) > 0) //인자로 받은 파일 디스크립터가 가리키는 파일에서 한글자씩 읽는다.
	{
		if(c == ':') // ':'나왔을 경우 그만
			break;
		
		result[idx++] = c; //result에 읽은거 한글자씩 저장한다.
	}
	if(result[strlen(result) - 1] == '\n') //result의 마지막 글자가 개행문자일경우
		result[strlen(result) - 1] = '\0'; //개행문자를 널로 바꿔준다.

	return result; //그 결과 리턴
}

int score_blank(char *id, char *filename)
{
	char tokens[TOKEN_CNT][MINLEN];
	node *std_root = NULL, *ans_root = NULL;
	int idx, start;
	char tmp[BUFLEN];
	char s_answer[BUFLEN], a_answer[BUFLEN];
	char qname[FILELEN];
	int fd_std, fd_ans;
	int result = true;
	int has_semicolon = false;

	memset(qname, 0, sizeof(qname)); //퀴즈 이름 들어갈 배열 비워준다.
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.'))); //마지막으로 나온 .뒤를 제외하고 filename에 들어간 문자열을 qname에 복사한다.

	sprintf(tmp, "%s/%s/%s", stuDir, id, filename); // 디렉토리/아이디/파일명 순으로 tmp에 저장한다.
	fd_std = open(tmp, O_RDONLY); //tmp에 경로가 들어간 그 파일을 읽기모드로 연다.
	strcpy(s_answer, get_answer(fd_std, s_answer)); //그 파일에 쓰인 답을 읽어서 s_answer에 저장한다.

	if(!strcmp(s_answer, "")){ //답이 없었을 경우
		close(fd_std); //파일 닫는다.
		return false;
	}

	if(!check_brackets(s_answer)){ //(와 ) 수가 같은지 조사
		close(fd_std); //다른경우 파일을 닫고
		return false; //false 리턴
	}

	strcpy(s_answer, ltrim(rtrim(s_answer))); //오른쪽 자르고 왼쪽 자른거 s_answer에 복사

	if(s_answer[strlen(s_answer) - 1] == ';'){ //마지막 글자가 세미콜론일 경우
		has_semicolon = true; //세미콜론이 있다는 것을 표시하고
		s_answer[strlen(s_answer) - 1] = '\0'; //세미콜론 대신에 널문자를 넣는다.
	}

	if(!make_tokens(s_answer, tokens)){
		close(fd_std);
		return false;
	}

	idx = 0;
	std_root = make_tree(std_root, tokens, &idx, 0);

	sprintf(tmp, "%s/%s/%s", ansDir, qname, filename);
	fd_ans = open(tmp, O_RDONLY);

	while(1)
	{
		ans_root = NULL;
		result = true;

		for(idx = 0; idx < TOKEN_CNT; idx++)
			memset(tokens[idx], 0, sizeof(tokens[idx]));

		strcpy(a_answer, get_answer(fd_ans, a_answer));

		if(!strcmp(a_answer, ""))
			break;

		strcpy(a_answer, ltrim(rtrim(a_answer)));

		if(has_semicolon == false){
			if(a_answer[strlen(a_answer) -1] == ';')
				continue;
		}

		else if(has_semicolon == true)
		{
			if(a_answer[strlen(a_answer) - 1] != ';')
				continue;
			else
				a_answer[strlen(a_answer) - 1] = '\0';
		}

		if(!make_tokens(a_answer, tokens))
			continue;

		idx = 0;
		ans_root = make_tree(ans_root, tokens, &idx, 0);

		compare_tree(std_root, ans_root, &result);

		if(result == true){
			close(fd_std);
			close(fd_ans);

			if(std_root != NULL)
				free_node(std_root);
			if(ans_root != NULL)
				free_node(ans_root);
			return true;

		}
	}
	
	close(fd_std);
	close(fd_ans);

	if(std_root != NULL)
		free_node(std_root);
	if(ans_root != NULL)
		free_node(ans_root);

	return false;
}

double score_program(char *id, char *filename)
{
	double compile;
	int result;

	compile = compile_program(id, filename);

	if(compile == ERROR || compile == false)
		return false;
	
	result = execute_program(id, filename);

	if(!result)
		return false;

	if(compile < 0)
		return compile;

	return true;
}

int is_thread(char *qname)
{
	int i;
	int size = sizeof(threadFiles) / sizeof(threadFiles[0]); //threadFiles 행 개수

	for(i = 0; i < size; i++){ //threadFiles 끝까지 검사
		if(!strcmp(threadFiles[i], qname)) //threadFiles에 qname이 존재할경우
			return true; //true 리턴
	}
	return false; //false 리턴
}

double compile_program(char *id, char *filename)
{
	int fd;
	char tmp_f[BUFLEN], tmp_e[BUFLEN];
	char command[BUFLEN];
	char qname[FILELEN];
	int isthread;
	off_t size;
	double result;

	memset(qname, 0, sizeof(qname)); //qname 초기화
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));//qname에 filename에서 .전까지 복사
	
	isthread = is_thread(qname); //threadFiles[]에 있는지 검사하여 그 결과 저장

	sprintf(tmp_f, "%s/%s", ansDir, filename);
	sprintf(tmp_e, "%s/%s.exe", ansDir, qname);

	if(tOption && isthread)
		sprintf(command, "gcc -o %s %s -lpthread", tmp_e, tmp_f);
	else
		sprintf(command, "gcc -o %s %s", tmp_e, tmp_f);

	sprintf(tmp_e, "%s/%s_error.txt", ansDir, qname);
	fd = creat(tmp_e, 0666);

	//STDERR를 fd도 가리키게 했다가
	//명령 실행하고 다시 STDERR 본래의 파일 디스크립터로 돌아오게 함
	redirection(command, fd, STDERR);
	size = lseek(fd, 0, SEEK_END);
	close(fd);
	unlink(tmp_e);

	if(size > 0)
		return false;

	sprintf(tmp_f, "%s/%s/%s", stuDir, id, filename);
	sprintf(tmp_e, "%s/%s/%s.stdexe", stuDir, id, qname);

	if(tOption && isthread)
		sprintf(command, "gcc -o %s %s -lpthread", tmp_e, tmp_f);
	else
		sprintf(command, "gcc -o %s %s", tmp_e, tmp_f);

	sprintf(tmp_f, "%s/%s/%s_error.txt", stuDir, id, qname);
	fd = creat(tmp_f, 0666);

	redirection(command, fd, STDERR);
	size = lseek(fd, 0, SEEK_END);
	close(fd);

	if(size > 0){
		if(eOption)
		{
			sprintf(tmp_e, "%s/%s", errorDir, id);
			if(access(tmp_e, F_OK) < 0)
				mkdir(tmp_e, 0755);

			sprintf(tmp_e, "%s/%s/%s_error.txt", errorDir, id, qname);
			rename(tmp_f, tmp_e);

			result = check_error_warning(tmp_e);
		}
		else{ 
			result = check_error_warning(tmp_f);
			unlink(tmp_f);
		}

		return result;
	}

	unlink(tmp_f);
	return true;
}

double check_error_warning(char *filename)
{
	FILE *fp;
	char tmp[BUFLEN];
	double warning = 0;

	if((fp = fopen(filename, "r")) == NULL){
		fprintf(stderr, "fopen error for %s\n", filename);
		return false;
	}

	while(fscanf(fp, "%s", tmp) > 0){
		if(!strcmp(tmp, "error:"))
			return ERROR;
		else if(!strcmp(tmp, "warning:"))
			warning += WARNING;
	}

	return warning;
}

int execute_program(char *id, char *filename)
{
	char std_fname[BUFLEN], ans_fname[BUFLEN];
	char tmp[BUFLEN];
	char qname[FILELEN];
	time_t start, end;
	pid_t pid;
	int fd;

	memset(qname, 0, sizeof(qname));
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));

	sprintf(ans_fname, "%s/%s.stdout", ansDir, qname);
	fd = creat(ans_fname, 0666);

	sprintf(tmp, "%s/%s.exe", ansDir, qname);
	redirection(tmp, fd, STDOUT);
	close(fd);

	sprintf(std_fname, "%s/%s/%s.stdout", stuDir, id, qname);
	fd = creat(std_fname, 0666);

	sprintf(tmp, "%s/%s/%s.stdexe &", stuDir, id, qname);

	start = time(NULL);
	redirection(tmp, fd, STDOUT);
	
	sprintf(tmp, "%s.stdexe", qname);
	while((pid = inBackground(tmp)) > 0){
		end = time(NULL);

		if(difftime(end, start) > OVER){
			kill(pid, SIGKILL);
			close(fd);
			return false;
		}
	}

	close(fd);

	return compare_resultfile(std_fname, ans_fname);
}

pid_t inBackground(char *name)
{
	pid_t pid;
	char command[64];
	char tmp[64];
	int fd;
	off_t size;
	
	memset(tmp, 0, sizeof(tmp));
	fd = open("background.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);

	sprintf(command, "ps | grep %s", name);
	redirection(command, fd, STDOUT);

	lseek(fd, 0, SEEK_SET);
	read(fd, tmp, sizeof(tmp));

	if(!strcmp(tmp, "")){
		unlink("background.txt");
		close(fd);
		return 0;
	}

	pid = atoi(strtok(tmp, " "));
	close(fd);

	unlink("background.txt");
	return pid;
}

int compare_resultfile(char *file1, char *file2)
{
	int fd1, fd2;
	char c1, c2;
	int len1, len2;

	fd1 = open(file1, O_RDONLY);
	fd2 = open(file2, O_RDONLY);

	while(1)
	{
		while((len1 = read(fd1, &c1, 1)) > 0){
			if(c1 == ' ') 
				continue;
			else 
				break;
		}
		while((len2 = read(fd2, &c2, 1)) > 0){
			if(c2 == ' ') 
				continue;
			else 
				break;
		}
		
		if(len1 == 0 && len2 == 0)
			break;

		to_lower_case(&c1);
		to_lower_case(&c2);

		if(c1 != c2){
			close(fd1);
			close(fd2);
			return false;
		}
	}
	close(fd1);
	close(fd2);
	return true;
}

void redirection(char *command, int new, int old)
{
	int saved;

	saved = dup(old);
	dup2(new, old);

	system(command);

	dup2(saved, old);
	close(saved);
}

int get_file_type(char *filename)
{
	char *extension = strrchr(filename, '.'); //filename에서 .이 나온 가장 오른쪽 위치 찾아 포인터로 리턴

	if(!strcmp(extension, ".txt")) //.txt파일일 경우
		return TEXTFILE; //TEXTFILE임을 리턴
	else if (!strcmp(extension, ".c")) //.c파일일 경우
		return CFILE; //CFILE임을 리턴
	else
		return -1; //둘다 아니면 -1
}

void rmdirs(const char *path)
{
	struct dirent *dirp;
	struct stat statbuf;
	DIR *dp;
	char tmp[518];
	
	if((dp = opendir(path)) == NULL)
		return;

	while((dirp = readdir(dp)) != NULL)
	{
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		sprintf(tmp, "%s/%s", path, dirp->d_name);

		if(lstat(tmp, &statbuf) == -1)
			continue;

		if(S_ISDIR(statbuf.st_mode))
			rmdirs(tmp);
		else
			unlink(tmp);
	}

	closedir(dp);
	rmdir(path);
}

void to_lower_case(char *c)
{
	if(*c >= 'A' && *c <= 'Z')
		*c = *c + 32;
}

//사용 방법 출력해주는 함수
void print_usage()
{
	printf("Usage : ssu_score <STUDENTDIR> <TRUEDIR> [OPTION]\n");
	printf("Option : \n");
	printf(" -e <DIRNAME>      print error on 'DIRNAME/ID/qname_error.txt' file \n");
	printf(" -t <QNAMES>       compile QNAME.C with -lpthread option\n");
	printf(" -h                print usage\n");
	printf(" -p                print student's score and total average\n");
	printf(" -c <IDS>          print ID's score\n");
}
