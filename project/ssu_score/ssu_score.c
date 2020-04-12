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

//문제이름과 점수로 구성된 ssu_scoreTable로 이루어진 배열
extern struct ssu_scoreTable score_table[QNUM];
//학생id을 저장하는 배열
extern char id_table[SNUM][10];

struct ssu_scoreTable score_table[QNUM];
char id_table[SNUM][10];

char stuDir[BUFLEN]; //학생들 학번으로 구성된 디렉토리들 담길 디렉토리
char ansDir[BUFLEN]; //문제 정답들이 담길 디렉토리
char errorDir[BUFLEN]; 
char threadFiles[ARGNUM][FILELEN];
char cIDs[ARGNUM][FILELEN];

int eOption = false;
int tOption = false;
int cOption = false;

void ssu_score(int argc, char *argv[])
{
	char saved_path[BUFLEN]; //경로 저장하는 배열
	int i;

	//인자 중에 -h 옵션이 있을 경우 사용방법 출력하고 끝내기
	for(i = 0; i < argc; i++){
		if(!strcmp(argv[i], "-h")){
			print_usage(); //사용방법 출력하는 함수 호출
			return;
		}
	}

	memset(saved_path, 0, BUFLEN); //경로 저장하는 배열 초기화
	if(argc >= 3 && strcmp(argv[1], "-c") != 0){ //인자가 2개 이상, c옵션이 아닐 경우
		strcpy(stuDir, argv[1]); //첫번째 인자를 stuDir로, 현 설계과제에선 STUDIR
		strcpy(ansDir, argv[2]); //두번째 인자를 ansDir로, 현 설계과제에선 ANSDIR
	}

	if(!check_option(argc, argv)) //옵션 체크
		exit(1); //설정하지 않은 옵션이 인자로 들어왔을 경우 종료.

/*/////////////////////////////삭제//////////////////////////////////////////////////
	if(!eOption && !tOption && cOption){ //c옵션만 들어왔을 경우
		do_cOption(cIDs); //cOption에 대한 함수 실행
		return;
	}
////////////////////////////////////////////////////////////////////////////////// */

	getcwd(saved_path, BUFLEN); //saved_path에 현재 작업 디렉토리 경로를 저장한다.

	if(chdir(stuDir) < 0){ //STDDIR로 현재 작업 디렉토리 변경
		fprintf(stderr, "%s doesn't exist\n", stuDir);
		return;
	}
	getcwd(stuDir, BUFLEN); //stdDir에 현재 작업 디렉토리 경로(STDDIR)를 저장한다.

	chdir(saved_path); //다시 전 디렉토리로 빠져나온다.
	if(chdir(ansDir) < 0){ //ANSDIR로 현재 작업 디렉토리 변경
		fprintf(stderr, "%s doesn't exist\n", ansDir);
		return;
	}
	getcwd(ansDir, BUFLEN); //ansDir에 현재 작업 디렉토리 경로(ANS_DIR)를 저장한다.

	chdir(saved_path); //다시 전 디렉토리로 빠져나온다.

	set_scoreTable(ansDir); //문제의 점수를 저장하는 scoreTable을 설정하는 함수를 호출한다.
	set_idTable(stuDir); //학생 학번을 저장하는 idTable을 설정하는 함수를 호출한다.

	printf("grading student's test papers..\n");
	score_students(); //학생들의 점수를 채점하는 함수 호출

/*/////////////////////삭제////////////////////////				
	if(cOption) //c 옵션 켜져있을 경우
		do_cOption(cIDs); //c 옵션 함수 수행
//////////////////////////////////////////////// */				

	return;
}

//옵션 체크하는 함수 <수정>
int check_option(int argc, char *argv[])
{
	int i, j;
	int c;

	//e 옵션은 별도의 파라미터를 받는다. 
	//(-e를 만나면 자동으로 그 뒤에 붙은 문자열을 공백까지 탐색하여 이를 optarg에 복사)
	//t,h,c 옵션은 별도의 파라미터를 받지 않는다.
	//각각의 옵션을 파싱하기 위해 반복문을 수행한다.
/*///////////////////////수정<이전>////////////////////////
   while((c = getopt(argc, argv, "e:thpc")) != -1)
/////////////////////////수정<이후>///////////////////// */	
	while((c = getopt(argc, argv, "e:th")) != -1)
///////////////////////////////////////////////////////////
	{
		switch(c){
			case 'e': //옵션 e
				eOption = true; //e옵션 킨다.
				strcpy(errorDir, optarg); //별도로 받은 파라미터를 errorDir로 설정한다.

				if(access(errorDir, F_OK) < 0) //설정한 errorDir 디렉토리가 없다면
					mkdir(errorDir, 0755); //디렉토리 _rwxr_xr_x권한으로 생성
				else{ //존재한다면
					rmdirs(errorDir); //해당 디렉토리 지우고 
					mkdir(errorDir, 0755); //재생성
				}
				break;
			case 't': //옵션 t
				tOption = true; //t옵션 킨다.
				i = optind; //다음번 처리될 옵션의 인덱스를 저장
				j = 0;

				while(i < argc && argv[i][0] != '-'){ //옵션 인자가 아닌 인자들 중

					if(j >= ARGNUM) //받은 가변인자의 개수가 5개가 넘었을 경우
						printf("Maximum Number of Argument Exceeded.  :: %s\n", argv[i]);
					else
						strcpy(threadFiles[j], argv[i]); //threadFiles에 그 인자를 추가한다.
					i++;  //다음 인자 체크하게
					j++;  //다음 인자 체크하게
				}
				break;
/*///////////////////////////////////////////삭제////////////////////////////////////////////				
			case 'p':                               
				pOption = true;                      
				break;                               
			
			case 'c': //옵션 c
				cOption = true; //c옵션 킨다.
				i = optind; //다음번 처리될 옵션의 인덱스를 저장
				j = 0;

				while(i < argc && argv[i][0] != '-'){ //옵션 인자가 아닌 인자들 중

					if(j >= ARGNUM) //받은 가변인자의 개수가 5개를 넘었을 경우
						printf("Maximum Number of Argument Exceeded.  :: %s\n", argv[i]);
					else
						strcpy(cIDs[j], argv[i]); //cID에 그 인자를 추가한다.
					i++; //다음 인자 체크하게
					j++; //다음 인자 체크하게
				}
				break;
//////////////////////////////////////////////////////////////////////////////////////////// */
			case '?': //설정하지 않은 옵션이 인자로 들어왔을 경우
				printf("Unkown option %c\n", optopt);
				return false;
		}
	}

	return true;
}

/*/////////////////////////////////////////////삭제//////////////////////////////////////////////////
//c옵션, 인자로 들어온 학생들의 점수를 출력
void do_cOption(char (*ids)[FILELEN])
{
	FILE *fp;
	char tmp[BUFLEN]; //학생 학번을 임시로 저장하는 배열
	int i = 0;
	char *p, *saved;

	if((fp = fopen("score.csv", "r")) == NULL){ //score.csv파일을 읽기모드로 연다.
		fprintf(stderr, "file open error for score.csv\n");
		return;
	}

	fscanf(fp, "%s\n", tmp); //문자열 하나 읽어서 tmp에 저장한다.

	while(fscanf(fp, "%s\n", tmp) != EOF) //파일의 끝에 도달할 때까지 읽는다.
	{
		p = strtok(tmp, ","); //읽은 문자열 ','기준으로 자르고 맨 앞에거를 tmp에 저장한다.

		if(!is_exist(ids, tmp)) //그 학번이 인자로 들어온 학번들 중에 존재하지 않는 경우
			continue;

		printf("%s's score : ", tmp); 

		while((p = strtok(NULL, ",")) != NULL) //','로 자른 문자열이 나오지 않을 때까지 반복
			saved = p; //saved가 가장 끝에 위치한 문자열인 총점을 받고 나서 반복문을 나오게 된다.

		printf("%s\n", saved); //학생의 총점을 출력
	}
	fclose(fp); //파일을 닫는다.
}
///////////////////////////////////////////////////////////////////////////////////////////////// */

//해당 문자열이 배열 안에 존재하는지 검사하는 함수
int is_exist(char (*src)[FILELEN], char *target)
{
	int i = 0;

	while(1)
	{
		if(i >= ARGNUM) //가변인자 최대 개수 끝까지 검사했는데 존재하지 않았을 경우
			return false; //false 리턴
		else if(!strcmp(src[i], "")) //받은 가변인자 끝까지 검사했는데 존재하지 않았을 경우
			return false; //false 리턴
		else if(!strcmp(src[i++], target)) //일치하는 것 찾았을 경우
			return true; //true 리턴
	}
	return false;
}

//ANSDIR에 score_table.csv(문제의 배점테이블) 설정하는 함수
void set_scoreTable(char *ansDir)
{
	char filename[FILELEN]; //filename 들어갈 배열

	//ANSDIR/score_table.csv라는 경로를 filename에 저장
	sprintf(filename, "%s/%s", ansDir, "score_table.csv");

	if(access(filename, F_OK) == 0) //score_table.csv가 이미 존재할 경우
		read_scoreTable(filename);   //score_table.csv를 읽는 함수 호출
	else{                           //존재하지 않을 경우
		make_scoreTable(ansDir);     //score_table.csv를 ANSDIR에 생성하는 함수 호출
		write_scoreTable(filename);  //score_table.csv에 점수 작성하는 함수 호출
	}
}

//score_table.csv를 읽어서 score_table 배열에 정보를 저장해주는 함수
void read_scoreTable(char *path)
{
	FILE *fp;
	char qname[FILELEN]; //퀴즈 이름
	char score[BUFLEN];  //퀴즈 점수
	int idx = 0;

	if((fp = fopen(path, "r")) == NULL){ //읽기모드로 ANSDIR/score_table.csv 연다.
		fprintf(stderr, "file open error for %s\n", path);
		return ;
	}

	//ANSDIR/score_table에서 ','로 구분하여 앞을 퀴즈이름, 뒤를 퀴즈 점수에 저장
	while(fscanf(fp, "%[^,],%s\n", qname, score) != EOF){ //파일 다 읽을 때까지 반복
		strcpy(score_table[idx].qname, qname); //score_table.qname에 해당 퀴즈이름 저장
		score_table[idx++].score = atof(score); //score_table.score에 해당 퀴즈 점수 저장
	}

	fclose(fp); //파일 닫는다.
}

//scoreTable배열에 점수 저장해주는 함수
void make_scoreTable(char *ansDir)
{
	int type, num;
	double score, bscore, pscore; //점수, 빈칸점수, 프로그래밍점수 저장하는 변수
	struct dirent *dirp, *c_dirp; 
	DIR *dp, *c_dp;
	char tmp[BUFLEN];
	int idx = 0;
	int i;

	num = get_create_type(); //점수 어떻게 입력할지 유저에게 입력받음
                            //1이면 빈칸문제와 프로그래밍문제를 따로 입력받음
									 //2면 모든 퀴즈의 점수를 하나씩 따로 입력받음
	
	if(num == 1) //빈칸문제와 프로그램 문제 따로 점수 입력하는 경우
	{
		printf("Input value of blank question : ");
		scanf("%lf", &bscore); //빈칸 문제의 점수를 담는 bscore에 점수 저장
		printf("Input value of program question : ");
		scanf("%lf", &pscore); //프로그램 문제의 점수를 담는 pscore에 점수 저장
	}

	if((dp = opendir(ansDir)) == NULL){ //ANSDIR를 연다.
		fprintf(stderr, "open dir error for %s\n", ansDir);
		return;
	}	

	while((dirp = readdir(dp)) != NULL) //ANSDIR 안에 있는 것을 하나씩 읽는다.
	{
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, "..")) //'.','..'은 넘어가고
			continue;

/*////////////////////////////////////수정 전//////////////////////////////////////////////////
		sprintf(tmp, "%s/%s", ansDir, dirp->d_name); //"ANSDIR/읽은것"을 tmp에 저장한다.       
                                                                                           
		if((c_dp = opendir(tmp)) == NULL){ //"ANSDIR/읽은 폴더"를 open한다.                    
			fprintf(stderr, "open dir error for %s\n", tmp);                                    
			return;                                                                             
		}                                                                                      
                                                                                           
		while((c_dirp = readdir(c_dp)) != NULL) //"ANSDIR/읽은 폴더"의 내부에 있는 파일 읽는다.
		{                                                                                      
			if(!strcmp(c_dirp->d_name, ".") || !strcmp(c_dirp->d_name, "..")) //'.','.." 넘어감 
				continue;                                                                        
                                                                                           
			if((type = get_file_type(c_dirp->d_name)) < 0) //읽은 파일의 문제 유형을 받는다.    
				continue;                                                                        
                                                                                           
			//읽은 파일의 이름을 score_table.qname에 저장하고, 다음인덱스로 넘긴다.             
			strcpy(score_table[idx++].qname, c_dirp->d_name);                                   
		}                                                                                      
                                                                                           
		closedir(c_dp); //"ANSDIR/읽은 폴더"를 닫는다.                                         
	}                                                                                         
                                                                                           
//////////////////////////////////////수정 후//////////////////////////////////////////////// */
	
	if((type = get_file_type(dirp->d_name)) < 0) //읽은 파일의 문제 유형을 받는다.
		continue;

   //읽은 파일의 이름을 score_table.qname에 저장하고, 다음인덱스로 넘긴다.        
	strcpy(score_table[idx++].qname, dirp->d_name);                                   
	
	}
	closedir(dp); //ANSDIR 닫는다.
///////////////////////////////////////////////////////////////////////////////////////////////
	
	sort_scoreTable(idx); //score_table 문제 번호 오름차순으로 정렬해준다.

	for(i = 0; i < idx; i++) //score_table 훑으면서 문제 유형에 따라 점수를 배정해준다.
	{
		type = get_file_type(score_table[i].qname); //문제의 유형(.txt, .c) 알아내는 함수 호출

		if(num == 1) //빈칸문제와 프로그램 문제 따로 점수 입력하는 경우
		{
			if(type == TEXTFILE)   //.txt 파일일 경우
				score = bscore;     //빈칸점수 할당
			else if(type == CFILE) //.c 파일일 경우
				score = pscore;     //프로그램 점수 할당
		}
		else if(num == 2) //모든 문제의 점수를 따로 하나씩 입력하는 경우
		{
			printf("Input of %s: ", score_table[i].qname);
			scanf("%lf", &score); //점수 할당
		}

		score_table[i].score = score; //위에서 할당된 점수를 score_table.score에 저장한다.
	}
}

//score_table.csv에 score_table에 저장된 점수를 써주는 함수
void write_scoreTable(char *filename)
{
	int fd;
	char tmp[BUFLEN];
	int i;
	int num = sizeof(score_table) / sizeof(score_table[0]); //score_table의 한 행의 사이즈 계산

	if((fd = creat(filename, 0666)) < 0){ //score_table.csv를 _rw_rw_rw 권한으로 생성한다.
		fprintf(stderr, "creat error for %s\n", filename);
		return;
	}

	for(i = 0; i < num; i++) //score_table을 전체 반복한다.
	{
		if(score_table[i].score == 0) //score_table에 문제 점수가 0점일 경우 그만
			break;
		
		//tmp에 "문제,점수" 꼴로 저장해준다.
		sprintf(tmp, "%s,%.2f\n", score_table[i].qname, score_table[i].score);
		//tmp에 저장된 값을 fd에 써준다.
		write(fd, tmp, strlen(tmp));
	}

	close(fd); //score_table.csv를 닫는다.
}

//학생 학번을 저장하는 id_table을 설정하는 함수 
void set_idTable(char *stuDir)
{
	struct stat statbuf;  //파일의 정보를 저장하는 구조체
	struct dirent *dirp;  //디렉토리 내의 파일을 가리키는 포인터
	DIR *dp;              //디렉토리 가리키는 포인터
	char tmp[BUFLEN];
	int num = 0;

	if((dp = opendir(stuDir)) == NULL){ //STUDIR을 연다.
		fprintf(stderr, "opendir error for %s\n", stuDir);
		exit(1);
	}

	while((dirp = readdir(dp)) != NULL){ //STUDIR 내의 파일을 읽는다.
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, "..")) //'.','..'은 넘어간다.
			continue;

		//"STUDIR/읽은파일" 을 tmp에 저장한다.
		sprintf(tmp, "%s/%s", stuDir, dirp->d_name);
		stat(tmp, &statbuf); //tmp에 저장한 파일의 정보를 저장한다.

		if(S_ISDIR(statbuf.st_mode)) //저장한 파일이 디렉토리일 경우
			strcpy(id_table[num++], dirp->d_name); //id_table에 파일 이름 저장하고 다음 인덱스로
		else //아닐경우
			continue; //다음 파일에 대하여 검사
	}

	sort_idTable(num); //id_table을 학번 오름차순으로 정렬하는 함수 호출
}

//id_table을 학번 오름차순으로 정렬하는 함수
void sort_idTable(int size)
{
	int i, j;
	char tmp[10]; //학번 담을 임시 배열

	//bubble sort방식으로 id_table을 학번 오름차순으로 정렬해준다.
	for(i = 0; i < size - 1; i++){
		for(j = 0; j < size - 1 -i; j++){
			if(strcmp(id_table[j], id_table[j+1]) > 0){
				strcpy(tmp, id_table[j]);
				strcpy(id_table[j], id_table[j+1]);
				strcpy(id_table[j+1], tmp);
			}
		}
	}
}

//score_table 문제 번호 오름차순으로 정렬해주는 함수
void sort_scoreTable(int size)
{
	int i, j;
	struct ssu_scoreTable tmp;
	int num1_1, num1_2; // ex) 1-3 이라하면 num1_1은 1, num 1_2는 3
	int num2_1, num2_2;
	
	
	//bubble sort 활용하여 score_table을 정렬해준다.
	for(i = 0; i < size - 1; i++){
		for(j = 0; j < size - 1 - i; j++){

			//연속된 두 인덱스의 qname을 모두 정수로 바꾸어서 저장해준다.
			get_qname_number(score_table[j].qname, &num1_1, &num1_2);
			get_qname_number(score_table[j+1].qname, &num2_1, &num2_2);

			//앞에게 클경우 or 앞에거 똑같지만 뒤에거가 클경우
			if((num1_1 > num2_1) || ((num1_1 == num2_1) && (num1_2 > num2_2))){
				
				//아래 세줄은 두 인덱스의 qname을 switching 해주는 과정
				memcpy(&tmp, &score_table[j], sizeof(score_table[0]));
				memcpy(&score_table[j], &score_table[j+1], sizeof(score_table[0]));
				memcpy(&score_table[j+1], &tmp, sizeof(score_table[0]));
			}
		}
	}
}

//qname에 있는 문제 번호를 받아 '-'앞에걸 num1으로, 뒤에걸 num2로 설정해주는 함수
void get_qname_number(char *qname, int *num1, int *num2)
{
	char *p;
	char dup[FILELEN]; //qname을 복사해서 넣어주기 위한 변수

	strncpy(dup, qname, strlen(qname));
	*num1 = atoi(strtok(dup, "-.")); //'-' 앞에있는걸 정수로 바꾸어 num1에 저장해준다.
	
	p = strtok(NULL, "-."); //'-' 뒤에거 검사
	if(p == NULL)           //없으면
		*num2 = 0;           //num2 = 0
	else                    //있으면
		*num2 = atoi(p);     //정수로 바꾸어 num2에 저장해준다.
}

//score_table.csv 존재하지 않을 때 빈칸문제와 프로그램 문제에 점수 넣는 방식 입력받기
int get_create_type()
{
	int num;

	while(1)
	{
		printf("score_table.csv file doesn't exist in TREUDIR!\n");
		printf("1. input blank question and program question's score. ex) 0.5 1\n");
		printf("2. input all question's score. ex) Input value of 1-1: 0.1\n");
		printf("select type >> ");
		scanf("%d", &num);

		if(num != 1 && num != 2) //이상한거 입력했을 경우 다시 입력받게
			printf("not correct number!\n");
		else //제대로 입력했을 경우 반복 끝낸다.
			break;
	}

	return num; //받은 입력값 리턴
}

//학생들의 점수를 채점하는 함수
void score_students()
{
	double score = 0;
	int num;
	int fd;
	char tmp[BUFLEN];
	int size = sizeof(id_table) / sizeof(id_table[0]); //id_table의 전체 행 개수 계산

	if((fd = creat("score.csv", 0666)) < 0){ //score.csv 파일을 생성한다.
		fprintf(stderr, "creat error for score.csv");
		return;
	}
	write_first_row(fd); //score.csv의 첫 행을 작성해주는 함수

	for(num = 0; num < size; num++) //id_table의 모든 행 반복
	{
		if(!strcmp(id_table[num], "")) //해당 행이 비어있을 경우 그만
			break;
		
		//tmp에 학번을 "학번," 꼴로 저장해준다.
		sprintf(tmp, "%s,", id_table[num]);
		//score.csv에 "학번,"을 써준다.
		write(fd, tmp, strlen(tmp)); 
		
		//각 학생의 점수를 채점하는 함수 호출
		//각 학생의 점수를 score에 더해주면서 합해준다.
		score += score_student(fd, id_table[num]);
	}
/*//////////////////////////수정 전///////////////////////////////////	
	if(pOption)
		printf("Total average : %.2f\n", score / num);

////////////////////////////수정 후//////////////////////////////// */		
	
	//합한 점수를 총 학생 명수로 나누어 평점을 출력한다.
	printf("Total average : %.2f\n", score / num);
//////////////////////////////////////////////////////////////////////	
	
	close(fd); //score.csv를 닫는다.
}

//각 학생의 점수를 채점하는 함수, id_table의 한 행(학번)을 인자로 갖고온다.
double score_student(int fd, char *id)
{
	int type;
	double result;
	double score = 0;
	int i;
	char tmp[BUFLEN];
	int size = sizeof(score_table) / sizeof(score_table[0]); //score_table의 전체 행 개수 저장

	//score_table의 모든 행 반복
	for(i = 0; i < size ; i++)
	{
		if(score_table[i].score == 0) //해당 행의 score가 0이면 그만
			break;
		
		//tmp에 "STUDIR/학번/문제번호"를 저장한다.
		sprintf(tmp, "%s/%s/%s", stuDir, id, score_table[i].qname);

		if(access(tmp, F_OK) < 0) //STUDOR/학번/문제번호"에 접근 가능한가
			result = false;
		else //가능할 경우 
		{
			if((type = get_file_type(score_table[i].qname)) < 0) //해당 문제의 유형 검사
				continue;
			
			if(type == TEXTFILE)   //빈칸문제일 경우
				result = score_blank(id, score_table[i].qname); //빈칸문제 채점결과 저장
			else if(type == CFILE) //프로그램 문제일 경우
				result = score_program(id, score_table[i].qname); //프로그램문제 채점결과 저장
		}

		if(result == false) //틀렸을 경우
			write(fd, "0,", 2); //score.csv에 "0," 써준다 (틀렸으니 0점)
		else{
			if(result == true){ //정답일 경우
				score += score_table[i].score; //해당 문제의 점수를 score에 더해준다.
				sprintf(tmp, "%.2f,", score_table[i].score); //tmp에 "점수," 써준다.
			}
			else if(result < 0){ //실행은 되는데, warning이 떴을 경우
				score = score + score_table[i].score + result; //warning떠서 감점된 점수로 더해준다.
				sprintf(tmp, "%.2f,", score_table[i].score + result); //tmp에 "점수," 써준다.
			}
			write(fd, tmp, strlen(tmp)); //score.csv에 "점수," 써준다.
		}
	}
/*//////////////////////수정 전/////////////////////////////////	
	if(pOption)
		printf("%s is finished.. score : %.2f\n", id, score); 
	else
		printf("%s is finished..\n", id);
////////////////////////수정 후////////////////////////////// */

	//채점이 끝났음과 함께 채점한 점수를 출력해준다.
	printf("%s is finished.. score : %.2f\n", id, score);

////////////////////////////////////////////////////////////////	
	
	//모든 문제를 채점한 결과를 tmp에 써준다.
	sprintf(tmp, "%.2f\n", score);
	//tmp에 써준 모든 문제를 채점한 결과를 score.csv에 써준다.
	write(fd, tmp, strlen(tmp));

	return score; //해당 학생의 총점을 리턴해준다.
}

//score.csv의 첫행을 채워주는 함수
void write_first_row(int fd)
{
	int i;
	char tmp[BUFLEN];
	int size = sizeof(score_table) / sizeof(score_table[0]); //score_table의 전체 행 개수 저장

	write(fd, ",", 1); //score.csv에 (0,0)을 비워주기 위해서 ','를 쓴다.

	for(i = 0; i < size; i++){ //score_table의 모든 행 반복
		if(score_table[i].score == 0) //해당 행의 점수가 0일경우 중단
			break;
		
		//해당 행의 문제 번호를 tmp에 "문제번호," 꼴로 저장
		sprintf(tmp, "%s,", score_table[i].qname);
		//score.csv에 "문제번호,"를 써준다.
		write(fd, tmp, strlen(tmp));
	}
	write(fd, "sum\n", 4); //모든 문제 번호 써준뒤 끝에 sum도 추가해준다.
}

//문제번호.txt를 가리키는 파일 디스크립터와
//문제번호.txt에서 읽은 ':'전까지의 답을 result에 저장해서
//리턴해주는 함수
char *get_answer(int fd, char *result)
{
	char c;
	int idx = 0;
	
	//두번 째 인자로 받은 배열을 0으로 초기화해준다.
	memset(result, 0, BUFLEN);
	while(read(fd, &c, 1) > 0) //STUDIR/학번/문제번호.txt 파일에서 한글자씩 읽는다.
	{ 
		if(c == ':') //':'를 읽었을 경우 그만
			break;
		
		result[idx++] = c; //result배열에 읽은 한글자씩을 넣어준다.
	}
	if(result[strlen(result) - 1] == '\n') //result에 저장된 마지막 글자가 개행문자일 경우
		result[strlen(result) - 1] = '\0';  //개행문자를 널문자로 바꾸어준다.

	return result; //result에 STUDIR/학번/문제번호.txt에서 읽은 답을 저장해서 리턴해준다
}

//빈칸 문제를 채점하는 함수
//학번과 문제번호.txt를 인자로 받는다.
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

	memset(qname, 0, sizeof(qname)); //문제번호를 저장하는 qname 배열을 초기화한다.
	//인자로 받은 문제번호.txt에서 문제 번호만을 파싱해서 qname에 저장해준다.
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));

	//tmp에 STUDIR/학번/문제번호.txt 저장해준다.
	sprintf(tmp, "%s/%s/%s", stuDir, id, filename);
	//STUDIR/학번/문제번호.txt를 읽기 전용으로 열어준다.
	fd_std = open(tmp, O_RDONLY);
	//STUDIR/학번/문제번호.txt에 입력된 학생의 답을 읽어와서 s_answer에 저장해준다.
	strcpy(s_answer, get_answer(fd_std, s_answer));

	//s_answer(학생이 제출한 답)이 비어있을 경우
	if(!strcmp(s_answer, "")){
		close(fd_std); //STUDIR/학번/문제번호.txt를 닫고
		return false;  //틀렸다는 의미에서 false 리턴
	}

	//s_answer(학생이 제출한 답)의 여는 괄호 닫는 괄호 개수 동일한지 체크해서
	if(!check_brackets(s_answer)){ //다르면
		close(fd_std); //STUDIR/학번/문제번호.txt 닫고
		return false;  //정답이 아니기에 false 리턴
	}
	
	//s_answer(학생이 제출한 답)의 우측, 좌측 공백을 모두 제거해준다.
	strcpy(s_answer, ltrim(rtrim(s_answer)));

	if(s_answer[strlen(s_answer) - 1] == ';'){ //s_answer의 제일 끝 값이 ;일 경우
		has_semicolon = true;                   //;을 갖고 있다는 의미에서 has_semicolon 킨다.
		s_answer[strlen(s_answer) - 1] = '\0';  //;을 널문자로 대체
	}
	
	//s_answer(학생이 제출한 답)으로 tokens를 만든다.
	if(!make_tokens(s_answer, tokens)){ //만드는데 실패했을 경우
		close(fd_std); //STUDIR/학번/문제번호.txt 닫고
		return false;  //false 리턴
	}

	idx = 0;
	std_root = make_tree(std_root, tokens, &idx, 0); //토큰들로 트리를 만들어준다.

/*////////////////////////수정 전////////////////////////////
	sprintf(tmp, "%s/%s/%s", ansDir, qname, filename);
//////////////////////////수정 후///////////////////////// */	
	sprintf(tmp, "%s/%s", ansDir, filename);
///////////////////////////////////////////////////////// ///	
	
	fd_ans = open(tmp, O_RDONLY); //ANSDIR/문제번호.txt 읽기모드로 연다.

	while(1)
	{
		ans_root = NULL;
		result = true;
		
		//토큰을 다 검사하는데
		for(idx = 0; idx < TOKEN_CNT; idx++)
			memset(tokens[idx], 0, sizeof(tokens[idx])); //tokens를 초기화

		//ANSDIR/문제번호.txt에서 답을 읽어서 a_answer에 저장한다.
		strcpy(a_answer, get_answer(fd_ans, a_answer));
		
		if(!strcmp(a_answer, "")) //읽은 답이 없는 경우
			break; //반복 그만
		
		//a_answer의 좌,우측 공백을 잘라서 a_answer에 다시 넣어준다.
		strcpy(a_answer, ltrim(rtrim(a_answer)));
		
		
		if(has_semicolon == false){ //s_answer의 끝 값이 ;가 아닐 경우
			//a_answer의 마지막 글자가 ;일 경우
			if(a_answer[strlen(a_answer) -1] == ';')
				continue; //현재 반복 넘어가기
		}
		//s_answer의 끝 값이 ;일 경우
		else if(has_semicolon == true)
		{	
			//a_answer의 마지막 글자가 ;가 아닐 경우
			if(a_answer[strlen(a_answer) - 1] != ';')
				continue;
			//a_answer의 마지막 글자가 ;일 경우
			else
				a_answer[strlen(a_answer) - 1] = '\0'; //마지막 글자 널문자로 바꿈
		}
		
		//a_answer들로 토큰을 만든다.
		if(!make_tokens(a_answer, tokens))
			continue; //현재 반복 넘어가기

		idx = 0;
		//a_answer에서 얻은 토큰들로 트리를 만든다.
		ans_root = make_tree(ans_root, tokens, &idx, 0);
		
		//학생의 트리와 정답 트리를 비교하여 결과를 받는다.
		compare_tree(std_root, ans_root, &result);
		
		//정답일 경우
		if(result == true){
			close(fd_std); //학생 제출 파일을 닫는다.
			close(fd_ans); //정답 파일을 닫는다.

			if(std_root != NULL) //학생 답 트리가 있을 경우
				free_node(std_root); //그 트리 할당을 해제해준다.
			if(ans_root != NULL) //정답 트리가 있을 경우
				free_node(ans_root); //그 트리 할당을 해제해준다.
			return true; //true를 리턴해준다.

		}
	}
	
	close(fd_std); //학생 답 제출파일 닫는다.
	close(fd_ans); //정답 파일 닫는다.

	if(std_root != NULL) //학생 답 트리가 있을 경우
		free_node(std_root); //그 트리 할당을 해제해주고
	if(ans_root != NULL) //정답 트리가 있을 경우
		free_node(ans_root); //그 트리 할당을 해제해준다.

	return false; //false를 리턴해준다.
}

//프로그램문제를 채점하는 함수, 학번과 문제번호를 인자로 받아온다.
double score_program(char *id, char *filename)
{
	double compile;
	int result;

	compile = compile_program(id, filename); //해당 문제를 코딩한 .c 파일 컴파일해주는 함수 호출

	if(compile == ERROR || compile == false) //에러일 경우
		return false; //틀렸다는 의미에서 false 리턴
	
	//컴파일한 파일을 실행하고 그 결과를 학생이 제출한 것과 정답을 비교하여 결과 불러옴
	result = execute_program(id, filename);

	if(!result) //두 실행결과가 다를경우 학생이 제출한 것이 틀렸다.
		return false; 

	if(compile < 0) //compile이 0보다 작다는건 warning의 이유로 감점이 됐다는거
		return compile; //감점치를 리턴

	return true; //컴파일도 warning없이 잘 이루어졌고, 실행결과도 일치할 경우 맞았다. 
}

//문제번호 받아서 threadFiles[]에 있는지 없는지 검사하는 함수
int is_thread(char *qname)
{
	int i;
	int size = sizeof(threadFiles) / sizeof(threadFiles[0]); //threadFiles 전체행 개수 계산

	for(i = 0; i < size; i++){ //끝까지 반복하는데
		if(!strcmp(threadFiles[i], qname)) //인자로 받은 문제번호가 threadFiles[]에 있으면
			return true; //트루 리턴
	}
	return false;
}

//해당 문제를 코딩한 .c 파일을 컴파일한다.
double compile_program(char *id, char *filename)
{
	int fd;
	char tmp_f[BUFLEN], tmp_e[BUFLEN];
	char command[BUFLEN];
	char qname[FILELEN];
	int isthread;
	off_t size;
	double result;

	memset(qname, 0, sizeof(qname)); //퀴즈번호를 저장하는 qname배열 초기화
	//.c 확장자를 제외한 앞의 파일 이름을 받아 qname에 저장한다.
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));
	
	isthread = is_thread(qname); //해당 문제번호가 threadFiles[]에 있는지 검사하는 함수 호출

/*//////////////////////수정 전//////////////////////////////////
	//tmp_f에 "ANSDIR/문제번호/문제번호.확장자" 입력한다.
	sprintf(tmp_f, "%s/%s/%s", ansDir, qname, filename);
	//tmp_f에 "ANSDIR/문제번호/문제번호.exe" 입력한다.
	sprintf(tmp_e, "%s/%s/%s.exe", ansDir, qname, qname);
////////////////////////수정 후/////////////////////////////// */
	//tmp_f에 "ANSDIR/문제번호.확장자" 입력한다.
	sprintf(tmp_f, "%s/%s", ansDir, filename);
	//tmp_f에 "ANSDIR/문제번호.exe" 입력한다.
	sprintf(tmp_e, "%s/%s.exe", ansDir, qname);
/////////////////////////////////////////////////////////////////

	if(tOption && isthread) //t 옵션이 켜져있고, 해당 문제 번호가 threadFiles[]에 있다면
		sprintf(command, "gcc -o %s %s -lpthread", tmp_e, tmp_f); //-lpthread 옵션으로 컴파일
	else //아니라면
		sprintf(command, "gcc -o %s %s", tmp_e, tmp_f); //그냥 컴파일

/*//////////////////////수정 전//////////////////////////////////
	//tmp_e에 "ANSDIR/문제번호/문제번호_error.txt" 입력한다.
	sprintf(tmp_e, "%s/%s/%s_error.txt", ansDir, qname, qname);
////////////////////////수정 후/////////////////////////////// */
	//tmp_e에 "ANSDIR/문제번호_error.txt" 입력한다.
	sprintf(tmp_e, "%s/%s_error.txt", ansDir, qname);
/////////////////////////////////////////////////////////////////
	fd = creat(tmp_e, 0666); //ANSDIR/문제번호_error.txt 생성한다.

	//STDERR을 fd도 가러리키게 했다가
	//command에 입력한 명령을 실행하고 다시 STDERR 본래의 파일 디스크립터로 돌아오게 함
	redirection(command, fd, STDERR);

	size = lseek(fd, 0, SEEK_END); //ANSDIR/문제번호_error.txt의 파일크기 측정
	close(fd); //ANSDIR/문제번호_error.txt 닫는다.
	unlink(tmp_e); //ANSDIR/문제번호_error.txt의 link count하나 줄인다(삭제)

	if(size > 0) //ANSDIR/문제번호_error.txt에 에러메세지 들어있을 경우
		return false; //컴파일 실패했다.
	
	//tmp_f에 "STUDIR/학번/문제번호.확장자" 입력한다.
	sprintf(tmp_f, "%s/%s/%s", stuDir, id, filename);
	//tmp_e에 "STUDIR/학번/문제번호.stdexe" 입력한다.
	sprintf(tmp_e, "%s/%s/%s.stdexe", stuDir, id, qname);

	if(tOption && isthread) //t 옵션이 켜져있고, 해당 문제 번호가 threadFiles[]에 있다면
		sprintf(command, "gcc -o %s %s -lpthread", tmp_e, tmp_f); //-lpthread 옵션으로 컴파일
	else
		sprintf(command, "gcc -o %s %s", tmp_e, tmp_f); //그냥 컴파일

	//tmp_f에 "STUDIR/학번/문제번호_error.txt" 입력한다.
	sprintf(tmp_f, "%s/%s/%s_error.txt", stuDir, id, qname);
	fd = creat(tmp_f, 0666); //STUDIR/학번/문제번호_error.txt 생성한다.

	//STDERR을 fd도 가리키게 했다가
	//command에 입력한 명령을 실행하고 다시 STDERR 본래의 파일 디스크립터로 돌아오게 함
	redirection(command, fd, STDERR);

	size = lseek(fd, 0, SEEK_END); //STUDIR/학번/문제번호_error.txt의 파일크기 측정
	close(fd); //STUDIR/학번/문제번호_error.txt 닫는다.

	if(size > 0){ //에러메세지 들어있을 경우
		if(eOption) //e옵션 켜져있을 경우
		{	//tmp_e에 "error/학번" 입력한다. (error는 내가 입력한 파라미터)
			sprintf(tmp_e, "%s/%s", errorDir, id);
			if(access(tmp_e, F_OK) < 0) //"error/학번"이 없다면
				mkdir(tmp_e, 0755); //"error/학번" 디렉토리를 만든다.
			
			//tmp_e에 "error/학번/문제번호_error.txt" 입력한다.
			sprintf(tmp_e, "%s/%s/%s_error.txt", errorDir, id, qname);
			//STUDIR/학번/문제번호_error.txt를 error/학번/문제번호_error.txt로 이동시킨다.
			rename(tmp_f, tmp_e);

			result = check_error_warning(tmp_e); //에러메세지 체크하는 함수 호출
		}
		else{ //e옵션 없을 경우
			result = check_error_warning(tmp_f); //에러메세지 체크하는 함수 호출
			unlink(tmp_f); //STUDIR/학번/문제번호_error.txt의 link count 하나 줄인다(삭제한다)
		}

		return result;
	}
	
	//에러메세지 없을경우
	unlink(tmp_f); //STUDIR/학번/문제번호_error.txt의 link count 하나 줄인다(삭제)
	return true; //컴파일 된다는 의미에서 true 리턴
}

//문제번호_error.txt에 에러메세지 체크한다.
double check_error_warning(char *filename)
{
	FILE *fp;
	char tmp[BUFLEN];
	double warning = 0;

	if((fp = fopen(filename, "r")) == NULL){ //문제번호_error.txt가 없다면
		fprintf(stderr, "fopen error for %s\n", filename);
		return false; //컴파일 실패했다는 의미에서 false 리턴
	}

	while(fscanf(fp, "%s", tmp) > 0){ //문제번호_error.txt 끝까지 읽으면서
		if(!strcmp(tmp, "error:")) //error: 가 있다면
			return ERROR;           //error라는 의미에서 ERROR(false)리턴
		else if(!strcmp(tmp, "warning:")) //warning: 이 있다면
			warning += WARNING;            //warning: 이 있을 때마다 고감점
	}

	return warning; //warning이 감점한 점수만큼 리턴
}

//학번과 문제번호.c 인자로 받아와서
//학생이 제출한 파일와 정답이 있는 파일의 컴파일해서 나온 결과
//실행해서 그 결과를 문제번호.stdexe에 저장하고
//그 결과들을 비교한 결과를 리턴
int execute_program(char *id, char *filename)
{
	char std_fname[BUFLEN], ans_fname[BUFLEN];
	char tmp[BUFLEN];
	char qname[FILELEN];
	time_t start, end;
	pid_t pid;
	int fd;

	memset(qname, 0, sizeof(qname)); //qname 배열을 초기화
	//filename에서 .앞에 있는 문제번호만을 파싱한다.
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));

/*//////////////////////////수정 전////////////////////////////////////////////
	//ans_fname에 "ANSDIR/문제번호/문제번호.stdout" 입력한다.
	sprintf(ans_fname, "%s/%s/%s.stdout", ansDir, qname, qname);
	fd = creat(ans_fname, 0666); //ANSDIR/문제번호/문제번호.stdout을 생성한다.

	//tmp에 "ANSDIR/문제번호/문제번호.exe" 입력한다.
	sprintf(tmp, "%s/%s/%s.exe", ansDir, qname, qname);
////////////////////////////수정 후///////////////////////////////////////// */
	//ans_fname에 "ANSDIR/문제번호.stdout" 입력한다.
	sprintf(ans_fname, "%s/%s.stdout", ansDir, qname);
	fd = creat(ans_fname, 0666); //ANSDIR/문제번호/문제번호.stdout을 생성한다.

	//tmp에 "ANSDIR/문제번호.exe" 입력한다.
	sprintf(tmp, "%s/%s.exe", ansDir, qname);
///////////////////////////////////////////////////////////////////////////////

	//STDOUT도 ANSDIR/문제번호.stdout을 가리키게 했다가
	//ANSDIR/문제번호.exe 실행하고 다시 STDOUT 본래의 파일 디스크립터로 돌아오게 함
	redirection(tmp, fd, STDOUT);
	close(fd); //ANSDIR/문제번호.stdout을 닫는다.

	//std_fname에 "STUDIR/학번/문제번호.stdout" 입력한다.
	sprintf(std_fname, "%s/%s/%s.stdout", stuDir, id, qname);
	fd = creat(std_fname, 0666); //STUDIR/학번/문제번호.stdout을 생성한다.

	//tmp에 STUDIR/학번/문제번호.stdexe &" 입력한다.
	sprintf(tmp, "%s/%s/%s.stdexe &", stuDir, id, qname);

	start = time(NULL); //time_t type의 현재 시간을 start에 저장

	//STDOUT도 "STDDIR/학번/문제번호.stdout"을 가리키게 했다가
	//STUDIR/학번/문제번호.stdexe & 실행하고 (명령어를 백그라운드로 동작시킴)
	//다시 STDOUT 본래의 파일 디스크립터로 돌아오게 함
	redirection(tmp, fd, STDOUT);
	
	//tmp에 문제번호.stdexe 입력한다.
	sprintf(tmp, "%s.stdexe", qname);
	
	//백그라운드에서 동작중일 때 5초이상 동작하면 그만두고 실패 리턴
	while((pid = inBackground(tmp)) > 0){
		end = time(NULL); //time_t type의 현재 시간을 end에 저장

		if(difftime(end, start) > OVER){ //5초 이상 경과했을 경우
			kill(pid, SIGKILL);           //해당 pid로 SIGKILL 시그널을 전송 (프로세스 종료)
			close(fd);                    //STUDIR/학번/문제번호.stdout을 닫는다.
			return false;                 //실패 리턴
		}
	}

	close(fd); //STUDIR/학번/문제번호.stdout을 닫는다.

	//학생이 제출한 소스코드 실행결과 파일과 정답 소스코드 실행결과 파일을 비교하여 리턴
	return compare_resultfile(std_fname, ans_fname);
}

//문제번호.stdexe를 인자로 받아와서
//백그라운드에서 동작중인 그 파일 프로세스 번호를 리턴한다.
pid_t inBackground(char *name)
{
	pid_t pid;
	char command[64];
	char tmp[64];
	int fd;
	off_t size;
	
	memset(tmp, 0, sizeof(tmp)); //tmp배열을 초기화
	
	//background.txt파일을 읽고쓰기모드로 여는데, 없으면 만들고 있으면 내용 싹 비운다.
	fd = open("background.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);

	//command에 "ps | grep 문제번호.stdexe"를 입력한다
	sprintf(command, "ps | grep %s", name);

	//STDOUT도 background.txt파일을 가리키게 했다가
	//ps | grep 문제번호.stdexe 실행하고
	//(현재 프로세스 목록중에서 문제번호.stdexe를 검색하는 명령)
	//다시 STDOUT 본래의 파일 디스크립터로 돌아오게 함
	redirection(command, fd, STDOUT);
	
	//background.txt의 제일 처음으로 offset 옮김
	lseek(fd, 0, SEEK_SET);
	//tmp에 tmp의 사이즈만큼 background.txt 읽어서 저장한다.
	read(fd, tmp, sizeof(tmp));
	
	if(!strcmp(tmp, "")){ //background에서 동작 중이 아닐경우
		unlink("background.txt"); //background.txt link count 하나 줄이고
		close(fd);                //background.txt 닫는다. (삭제됨)
		return 0;
	}

	//실행중인 경우 tmp에 저장된 문자열에서 공백 앞에거(pid) 파싱하여 저장
	pid = atoi(strtok(tmp, " "));
	close(fd); //background.txt 닫는다.

	unlink("background.txt"); //background.txt link count 하나 줄인다 (삭제됨)
	return pid;               //동작중인 프로세스 번호를 리턴한다.
}

//두 소스코드 실행결과 파일을 비교하는 함수
int compare_resultfile(char *file1, char *file2)
{
	int fd1, fd2;
	char c1, c2;
	int len1, len2;
	
	fd1 = open(file1, O_RDONLY); //첫번째 인자로 들어온 실행결과 파일을 읽기모드로 오픈
	fd2 = open(file2, O_RDONLY); //두번째 인자로 들어온 실행결과 파일을 읽기모드로 오픈

	while(1)
	{
		while((len1 = read(fd1, &c1, 1)) > 0){ //fd1에 해당하는 파일 한글자씩 읽는다.
			if(c1 == ' ') //공백은 뛰어넘고
				continue;
			else //나머지 글자는 읽다가 멈춘다.
				break;
		}
		while((len2 = read(fd2, &c2, 1)) > 0){ //fd2에 해당하는 파일 한글자씩 읽는다.
			if(c2 == ' ')  //공백은 뛰어넘고
				continue;
			else //나머지 글자는 읽다가 멈춘다.
				break;
		}
		
		if(len1 == 0 && len2 == 0) //둘다 읽기를 완료했을 경우 반복 그만한다.
			break;

		to_lower_case(&c1); //fd1에서 읽은 한글자를 소문자로 바꿔준다.
		to_lower_case(&c2); //fd2에서 읽은 한글자를 소문자로 바꿔준다.

		if(c1 != c2){ //두 글자가 다를경우 (실행결과가 다르다)
			close(fd1); //fd1에 해당하는 파일 닫는다.
			close(fd2); //fd2에 해당하는 파일 닫는다.
			return false; //학생이 제출한 답이 틀렸다고 할 수 있다.
		}
	}
	close(fd1); //fd1에 해당하는 파일 닫는다.
	close(fd2); //fd2에 해당하는 파일 닫는다.
	return true; //둘 다 끝까지 읽어도 차이가 없다는 점은 결과가 같다는거고, 맞았다고 할 수 있다.
}

//old가 가리키는 파일 saved도 가리키게 하고,
//old가 새로운 new 파일디스크립터를 가리키게 만들었다가
//명령 실행하고 다시 saved에 저장해놨던 예전 파일 디스크립터를 가리키게 만드는 함수 
void redirection(char *command, int new, int old)
{
	int saved;

	saved = dup(old); //old 파일 디스크립터 dup해서 saved도 old가 가리키는 파일 가리키게
	dup2(new, old); //new 파일 디스크립터 dup2해서 old도 new가 가리키는 파일 가리키게

	system(command); //인자로 받은 명령어 system에 입력

	dup2(saved, old); //saved 파일 디스크립터 dup2해서 old도 saved가 가리키는 파일 가리키게
	close(saved); //saved가 가리키는 파일 닫는다.
}

int get_file_type(char *filename)
{
	//가장 끝에 . 나타나는 위치를 &extention에 저장. (확장자 파싱)
	char *extension = strrchr(filename, '.');

	if(!strcmp(extension, ".txt")) //.txt 파일일 경우
		return TEXTFILE; //TEXTFILE 리턴
	else if (!strcmp(extension, ".c")) //.c 파일일 경우
		return CFILE; //CFILE 리턴
	else
		return -1; //둘다 아닐경우 -1리턴
}

//디렉토리 지우는 함수
void rmdirs(const char *path)
{
	struct dirent *dirp;
	struct stat statbuf;
	DIR *dp;
	char tmp[516];
	
	//인자로 받아온 폴더가 없으면 리턴하고 함수 끝냄
	if((dp = opendir(path)) == NULL)
		return;
	
	//해당 폴더 내부의 파일들을 읽는다
	while((dirp = readdir(dp)) != NULL)
	{
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, "..")) //'.'과".."는 넘어가고
			continue;

		//tmp에 "디렉토리/디렉토리 내의 파일"을 입력한다.
		sprintf(tmp, "%s/%s", path, dirp->d_name);
		
		//"디렉토리/디렉토리 내의 파일"의 정보를 statbuf에 저장하는데 에러가 발생했을 경우
		if(lstat(tmp, &statbuf) == -1)
			continue; //다음 파일에 대하여 반복 계속

		if(S_ISDIR(statbuf.st_mode)) //statbuf에 저장된 파일의 종류가 디렉토리일 경우에
			rmdirs(tmp); //재귀적으로 디렉토리 지우는 함수 호출
		else //디렉토리가 아닐경우
			unlink(tmp); //그 파일 지워버린다.
	}

	closedir(dp); //인자로 받아온 폴더를 닫고
	rmdir(path);  //그 폴더를 지운다.
}

//대문자를 소문자로 바꾸어주는 함수
void to_lower_case(char *c)
{
	if(*c >= 'A' && *c <= 'Z')
		*c = *c + 32;
}

//사용 방법 출력해주는 함수 <수정>
void print_usage()
{
	printf("Usage : ssu_score <STUDENTDIR> <TRUEDIR> [OPTION]\n");
	printf("Option : \n");
	//<추가> -m 옵션
	printf(" -m (미구현)       modify question's score\n");
	printf(" -e <DIRNAME>      print error on 'DIRNAME/ID/qname_error.txt' file \n");
	printf(" -t <QNAMES>       compile QNAME.C with -lpthread option\n");
	//<추가> -i 옵션
	printf(" -i <IDS> (미구현) print ID's wrong questions\n");
	printf(" -h                print usage\n");
	//<삭제> -c 옵션
	//printf(" -c <IDS>          print ID's score\n");
	//<삭제> -p 옵션
	//printf(" -p                print student's score and total average\n");
}
