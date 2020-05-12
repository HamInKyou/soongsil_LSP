#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

#define BUFFER_MAX 256
#define MAX_PATH 1024
#define ARRAY_MAX 100
#define ARG_MAX 6

typedef struct f_info{
	char pathname[MAX_PATH];
	char fname[BUFFER_MAX];
	time_t deletedtime;
	time_t mtime;
} Fileinfo;

void do_monitor();
int read_files(Fileinfo file_array[ARRAY_MAX], char *pathname, int *amount, int *index);
int read_trash_files_info(Fileinfo file_array[ARRAY_MAX], char *pathname, int *amount, int *index);
int daemon_init(void);
void do_delete(char *command);
void alarm_handler(int signo);
int rmdirs(const char *path, int force);
void do_size(char *command);
void do_recover(char *command);
int do_tree(char *pathname, int depth);
void do_help(void);

Fileinfo delete_file_info;

int rOption;
int iOption;
int lOption;

int main(void)
{
	char command[BUFFER_MAX];
	char command_first_arg[BUFFER_MAX];
	char *prompt = "20162527>";
	char pathname[MAX_PATH];

	int i = 0;
	int pid;

	//자식 프로세스 생성
	if ((pid = fork()) < 0) {
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if (pid == 0) { //자식의 경우
		do_monitor(); //monitor 하기
	}
	else{ //부모의 경우

		while(1){
			fputs(prompt, stdout); 
			memset(command, '\0', BUFFER_MAX); //command 널로 채워주기

			if (fgets(command, sizeof(command), stdin) == NULL) //명령어 입력받기
				break;

			command[strlen(command)-1] = '\0'; //개행문자 널로 바꿔주기

			i = 0;
			if(command[0] == '\0') //아무것도 입력 안했을 경우 다시 입력받기
				continue;
			else {
				while(command[i] != ' ' && command[i] != '\0'){ //첫번째 arg 받는 과정
					command_first_arg[i] = command[i];
					i++;
				}
				command_first_arg[i] = '\0';
			}

			if (strcmp(command_first_arg, "delete") == 0){
				do_delete(command);
			}
			else if (strcmp(command_first_arg, "size") == 0){
				do_size(command);
			}
			else if (strcmp(command_first_arg, "recover") == 0){
				do_recover(command);
			}
			else if (strcmp(command_first_arg, "tree") == 0){
				memset(pathname, '\0', MAX_PATH);
				strcpy(pathname, "check");
				do_tree(pathname, 0);
			}
			else if (strcmp(command_first_arg, "exit") == 0){
				printf("이상 프로그램을 종료합니다.\n");
				break;
			}
			else if (strcmp(command_first_arg, "help") == 0){
				do_help();
			}
			else {
				do_help();
			}
			printf("\n");
		}
	}
}
void do_monitor(){
	FILE *fp;
	time_t current_time;
	struct tm tm_time;

	Fileinfo base_file_array[ARRAY_MAX];
	Fileinfo new_file_array[ARRAY_MAX];

	char pathname[MAX_PATH];
	char tmp_pathname[MAX_PATH];
	char *fname = "log.txt";

	int base_files_amount = 0;
	int new_files_amount = 0;
	int index = 0;
	int is_in_array;
	int i, j;


	getcwd(tmp_pathname, PATH_MAX); //현재 디렉토리 경로 임시 저장

	if(daemon_init() < 0) { //데몬 프로세스 생성
		fprintf(stderr, "daemon_init failed\n");
		exit(1);
	}

	chdir(tmp_pathname); //원래 디렉토리로 작업디렉토리 변경

	//log.txt 있으면 열고, 없으면 생성
	if((fp = fopen(fname, "a")) < 0){
		exit(0);
	}

	setvbuf(fp, NULL, _IONBF, 0); //버퍼 Non_buffered 모드로

	memset(pathname, '\0', MAX_PATH);
	strcpy(pathname, "check"); //pathname check로 초기화
	index = 0; //base_file_array 배열에 차례대로 저장하기 위해서
	read_files(base_file_array, pathname, &base_files_amount, &index); //비교의 기준이 되는 배열에 파일들 읽어서 저장

	while(1){
		current_time = time(NULL); //현재시간 저장
		memset(pathname, '\0', MAX_PATH);
		strcpy(pathname, "check"); //pathname check로 초기화
		index = 0; //base_file_array 배열에 차례대로 저장하기 위해서
		new_files_amount = 0; //new_file_array 배열에 들어간 모든 파일의 수
		read_files(new_file_array, pathname, &new_files_amount, &index); //비교의 대상이 되는 배열에 파일들 읽어서 저장


		if(base_files_amount == new_files_amount){ //파일들 수가 같은 경우
			for(i = 0; i < base_files_amount; i++){ //모든 파일들 체크한다.
				if(base_file_array[i].mtime != new_file_array[i].mtime){ //최종 수정시간이 다른 경우
					localtime_r(&new_file_array[i].mtime, &tm_time);
					fprintf(fp, "[%04d-%02d-%02d ",tm_time.tm_year+1900, tm_time.tm_mon+1, tm_time.tm_mday);
					fprintf(fp, "%02d:%02d:%02d]",tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
					fprintf(fp, "[modify_%s]\n",new_file_array[i].fname);
				}
			}
		}
		else { //파일들 수가 다를 경우
			//삭제된 파일들 검사
			for(i = 0; i < base_files_amount; i++){ //기준 배열에서 한 파일이
				is_in_array = 0; //파일이 있는지 없는지 체크하는 변수 0으로
				for(j = 0; j < new_files_amount; j++){ //비교 대상 배열 모든거 뒤져봤을 때
					if(!strcmp(base_file_array[i].pathname,new_file_array[j].pathname)){ //존재할 경우
						is_in_array = 1;
						break; //다음 파일 비교하게 내부 반복문 나가기
					}
				}
				if(is_in_array == 0){ //존재하지 않을 경우
					localtime_r(&current_time, &tm_time); //현재 시간 tm 구조체의 형태로
					fprintf(fp, "[%04d-%02d-%02d ",tm_time.tm_year+1900, tm_time.tm_mon+1, tm_time.tm_mday);
					fprintf(fp, "%02d:%02d:%02d]",tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
					fprintf(fp, "[delete_%s]\n",base_file_array[i].fname);
				}
			}

			//생성된 파일들 검사
			for(i = 0; i < new_files_amount; i++){ //비교 대상 배열에서 한 파일이
				is_in_array = 0; //파일이 있는지 없는지 체크하는 변수 0으로
				for(j = 0; j < base_files_amount; j++){ //기준 배열 모든거 뒤져봤을 때
					if(!strcmp(new_file_array[i].pathname,base_file_array[j].pathname)){ //존재할 경우
						is_in_array = 1;
						break; //다음 파일 비교하게 내부 반복문 나가기
					}
				}
				if(is_in_array == 0){ //존재하지 않을 경우
					localtime_r(&current_time, &tm_time); //현재 시간 tm 구조체의 형태로
					fprintf(fp, "[%04d-%02d-%02d ",tm_time.tm_year+1900, tm_time.tm_mon+1, tm_time.tm_mday);
					fprintf(fp, "%02d:%02d:%02d]",tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
					fprintf(fp, "[create_%s]\n",new_file_array[i].fname);
				}
			}
		}
		for(i = 0; i < ARRAY_MAX; i++){ //기준 배열을 새 배열로 바꿔주기
			base_file_array[i] = new_file_array[i];
		}
		base_files_amount = new_files_amount;

		sleep(1);
	}
	exit(0);
}

int read_files(Fileinfo file_array[ARRAY_MAX], char *pathname, int *amount, int *index){
	struct dirent **dirp;
	struct stat statbuf;
	char *ptr;
	int count;

	int idx;

	ptr = pathname + strlen(pathname); //인자로 넘어온 경로의 끝으로 포인터 이동
	*ptr++ = '/'; //끝에 / 붙여준다. (경로 잇기)
	*ptr = '\0';

	if((count = scandir(pathname, &dirp, NULL, alphasort)) == -1){ //디렉토리의 파일들 읽어온다.
		fprintf(stderr, "Directory scan Error\n");
		exit(1);
	}

	//디렉토리 사이즈만큼 반복수행
	for (idx = 0; idx < count; idx++)
	{
		//'.'과 ".." 파일은 넘어가고
		if((!strcmp(dirp[idx]->d_name, ".")) || (!strcmp(dirp[idx]->d_name, ".."))){
			continue;
		}

		//경로의 끝에 현재 검사하려는 파일 이어준다.
		strcpy(ptr, dirp[idx]->d_name);
		if(strstr(dirp[idx]->d_name,".swp") != NULL)
			continue;
		lstat(pathname, &statbuf); //파일 정보 받기

		strcpy(file_array[*index].pathname, pathname); //배열에 파일 경로 넣기
		strcpy(file_array[*index].fname, dirp[idx]->d_name); //배열에 파일 이름 넣기
		file_array[*index].mtime = statbuf.st_mtime; //배열에 최종 수정시간 저장


		*amount += 1;
		*index += 1;

		if (S_ISDIR(statbuf.st_mode)){ //그 파일이 디렉토리이면
			read_files(file_array, pathname, amount, index); //재귀적으로 들어가서 다시 읽기
		}
	}

	for (idx = 0; idx < count; idx++) { //할당 해제
		free(dirp[idx]);
	}
	free(dirp); //할당 해제

	ptr[-1] = 0;
	return 0;
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

void do_delete(char *command){
	time_t current_time;
	time_t deleted_time;
	struct tm deleted_time_tm;

	Fileinfo file_array[ARRAY_MAX]; //파일들 다 읽어서 임시로 저장할 배열

	char tmp[BUFFER_MAX]; //command 임시로 저장하고 있을 문자열
	char *ptr;
	char arg[ARG_MAX][BUFFER_MAX]; //인자들 다 받은 배열
	char pathname[MAX_PATH];
	char arg_pathname[MAX_PATH]; //인자로 받은 상대경로 저장하기 위한 문자열
	char key_path_resolved[MAX_PATH]; //찾고싶은 파일 절대경로로 바꾼거
	char base_path_resolved[MAX_PATH]; //비교 대상이 되는 파일 절대 경로로 바꿔서 저장
	char yesorno;

	double diff_time; //입력받은 날짜, 시간의 차이를 초단위로 리턴

	int files_amount = 0;
	int index = 0;
	int is_in_array = 0;
	int i;

	rOption = 0; //r옵션 끄기
	iOption = 0; //i옵션 끄기
	
	signal(SIGALRM, alarm_handler); //알람 시그널에 대해 핸들러 설정
	memset(tmp, '\0', BUFFER_MAX); //명령어 임시로 담을 배열
	memcpy(tmp, command, strlen(command)); //임시로 담을 배열에 명령 옮겨준다.
	ptr = strtok(tmp, " "); //command 자른다.

	for(i = 0; i < ARG_MAX; i++){ //명령어 임시로 담을 배열 모두 널로 초기화
		memset(arg[i], '\0', BUFFER_MAX); 
	}

	i = 0;
	//파일명, 종료시간, 옵션 담을 배열에 저장
	while (ptr != NULL)
	{
		memcpy(arg[i], ptr, strlen(ptr));
		i++;
		ptr = strtok(NULL, " ");
	}

	if(arg[1][0] == '\0'){ //파일명 입력 안했을 경우
		fprintf(stderr, "파일명을 입력해주십시오");
		exit(1);
	}

	memset(delete_file_info.pathname, '\0', MAX_PATH); //삭제할 파일의 경로 저장될 문자열 초기화
	memset(delete_file_info.fname, '\0', BUFFER_MAX); //삭제할 파일의 이름 저장될 문자열 초기화
	
	memset(arg_pathname, '\0', MAX_PATH);
	if(!strncmp(arg[1], "/home", 5)){ //절대 경로로 입력받았을 경우
		strcpy(arg_pathname, arg[1]); //arg_pathname에 그대로 저장
	}
	else{ //상대 경로로 입력받았을 경우
		strcpy(arg_pathname, "check"); //arg_path check로 초기화

		ptr = arg_pathname + strlen(arg_pathname); //ptr이 제일 끝 가리키게
		*ptr++ = '/'; // 끝에 / 붙여줘서 경로 잇는다.
		strcpy(ptr, arg[1]); // check/입력한 상대경로 꼴로 바꿔준다.
	}

	realpath(arg_pathname, key_path_resolved); //입력한 파일 절대경로로 바꾼다.
	
	memset(pathname, '\0', MAX_PATH); //pathname 초기화
	strcpy(pathname, "check"); //pathname check로 초기화
	read_files(file_array, pathname, &files_amount, &index); //배열에 파일들 읽어서 저장
	for(i = 0; i < files_amount; i++){
		realpath(file_array[i].pathname, base_path_resolved); //파일 절대경로로 바꿔서 저장
		if(!strcmp(key_path_resolved, base_path_resolved)){ //파일 찾았을 경우
			memcpy(delete_file_info.pathname, base_path_resolved, strlen(base_path_resolved)); //삭제할 파일 절대경로 저장
			memcpy(delete_file_info.fname, file_array[i].fname, strlen(file_array[i].fname)); //삭제할 파일 이름 저장
			delete_file_info.mtime = file_array[i].mtime; //최종 수정시간 저장
			is_in_array = 1;
			break;
		}
	}
	if(is_in_array){ //파일을 찾았을 경우
		//다음 인자로 옵션이 바로 나왔거나 아무것도 없을 경우 바로 삭제하겠다는 뜻!
		if( arg[2][0] == '-' || arg[2][0] == '\0') { 
			current_time = time(NULL); //현재 시간 저장
			if(arg[2][1] == 'r')
				rOption = 1;
			else if(arg[2][1] == 'i')
				iOption = 1;

			if(arg[3][0] == '-' ) { //옵션이 두개 연속으로 나올 수도 있으니까
				if(arg[3][1] == 'r')
					rOption = 1;
				else if(arg[3][1] == 'i')
					iOption = 1;
			}

			alarm(0); // 전에 삭제 알람 걸어놓았다면 그 알림 없애기
			alarm_handler(SIGALRM); //핸들러를 직접적으로 바로 호출해서 삭제하도록

		}
		//시간 나올 차례에 시간을 입력 안했을 경우 그 날짜 시작되는 00시 00분 00초에 삭제하겠다는 뜻!
		else if ( arg[3][0] == '-' || arg[3][0] == '\0'){ 
			current_time = time(NULL); //현재 시간 저장
			ptr = strtok(arg[2], "-"); //입력 받은 날짜 '-' 기준으로 나눔
			i = 0;
			while( ptr != NULL)
			{
				if(i == 0)
					deleted_time_tm.tm_year = atoi(ptr) - 1900; //삭제할 년
				else if(i == 1)
					deleted_time_tm.tm_mon = atoi(ptr) - 1; //삭제할 월
				else if(i == 2)
					deleted_time_tm.tm_mday = atoi(ptr); //삭제할 일
				i++;
				ptr = strtok(NULL, "-");
			}

			deleted_time_tm.tm_hour = 0; //삭제할 시
			deleted_time_tm.tm_min = 0; //삭제할 분
			deleted_time_tm.tm_sec = 0; //삭제할 초

			deleted_time_tm.tm_isdst = 0; //썸머타임 사용 안함
			deleted_time = mktime( &deleted_time_tm ); //tm 타입을 time_t 타입으로 바꾸기

			diff_time = difftime( deleted_time, current_time ); //현재시간과 삭제 시간의 차이를 초로

			if(arg[3][0] == '-' ) { //옵션이 있는지 검사
				if(arg[3][1] == 'r')
					rOption = 1;
				else if(arg[3][1] == 'i')
					iOption = 1;
				if(arg[4][0] == '-' ) { //옵션이 두개 연속으로 나올 수도 있으니까
					if(arg[4][1] == 'r')
						rOption = 1;
					else if(arg[4][1] == 'i')
						iOption = 1;
				}
			}

			alarm(diff_time);
		}
		//날짜 시간 제대로 입력했을 경우	그 날짜, 시간에 삭제하겠다는 뜻!
		else { 
			current_time = time(NULL); //현재 시간 저장
			ptr = strtok(arg[2], "-"); //입력 받은 날짜 '-' 기준으로 나눔
			i = 0;
			while( ptr != NULL)
			{
				if(i == 0)
					deleted_time_tm.tm_year = atoi(ptr) - 1900; //삭제할 년
				else if(i == 1)
					deleted_time_tm.tm_mon = atoi(ptr) - 1; //삭제할 월
				else if(i == 2)
					deleted_time_tm.tm_mday = atoi(ptr); //삭제할 일
				i++;
				ptr = strtok(NULL, "-");
			}

			ptr = strtok(arg[3], ":"); //입력 받은 시간 ':' 기준으로 나눔
			i = 0;
			while( ptr != NULL)
			{
				if(i == 0)
					deleted_time_tm.tm_hour = atoi(ptr); //삭제할 시
				else if(i == 1)
					deleted_time_tm.tm_min = atoi(ptr); //삭제할 분
				else if(i == 2)
					deleted_time_tm.tm_sec = atoi(ptr); //삭제할 초
				i++;
				ptr = strtok(NULL, "-");
			}

			deleted_time_tm.tm_isdst = 0; //썸머타임 사용 안함
			deleted_time = mktime( &deleted_time_tm ); //tm 타입을 time_t 타입으로 바꾸기

			diff_time = difftime( deleted_time, current_time ); //현재시간과 삭제 시간의 차이를 초로
			
			if(arg[4][0] == '-') { //옵션이 있는지 검사
				if(arg[4][1] == 'r')
					rOption = 1;
				else if(arg[4][1] == 'i')
					iOption = 1;
				if(arg[5][0] == '-' ) { //옵션이 두개 연속으로 나올 수도 있으니까
					if(arg[5][1] == 'r')
						rOption = 1;
					else if(arg[5][1] == 'i')
						iOption = 1;
				}
			}
			alarm(diff_time);
		}
	}
	else{ //파일을 못 찾았을 경우
		fprintf(stderr, "there is no file [%s]\n", arg[1]);
		exit(1);
	}

}
void alarm_handler(int signo){
	FILE *fp;
	time_t current_time;
   time_t old_time;
	struct tm tm_time;
	
	Fileinfo deleted_file_array[ARRAY_MAX];
	
	char pathname[MAX_PATH];
	char want_delete_pathname[MAX_PATH];
	char new_pathname_files[MAX_PATH];
	char new_pathname_info[MAX_PATH];
	char yesorno;

	int old_file_index = 0;
	int index = 0;
	int amount = 0;
	int do_delete = 1;
	int i;

	if(rOption == 1) { //r옵션 켜져있을 경우
		printf("Delete [y/n]?");
		scanf("%c", &yesorno);
		getchar();
		if(yesorno == 'y' || yesorno == 'Y'){ //삭제할거라면
			do_delete = 1;
		}
		else{
			do_delete = 0;
		}
	}

	if(do_delete == 1){
		if( access("trash", F_OK) < 0 ) { //trash 폴더 없으면 생성
			mkdir("trash", 0777);
		}
	
		if( access("trash/files", F_OK) < 0 ) { //trash/files 없으면 생성
			mkdir("trash/files", 0777);
		}

		if( access("trash/info", F_OK) < 0 ) { //trash/info 없으면 생성
			mkdir("trash/info", 0777);
		}
		
		if(iOption == 1){ //i 옵션 켜져있을 경우
			rmdirs(delete_file_info.pathname, 1); //절대경로 받아온거 그 파일(혹은 디렉토리) 아예 삭제
		}
		else { //i 옵션 꺼져있을 경우
			i = 0;
			while(1){
				if(i == 0){ //최초 딜리트 파일
					sprintf(new_pathname_files, "%s/%s", "trash/files", delete_file_info.fname); //파일 이름 새 경로로
					sprintf(new_pathname_info, "%s/%s", "trash/info", delete_file_info.fname); //파일 이름 새 경로로
					if(!access(new_pathname_files, F_OK)){ //해당 파일이 이미 trash/files에 존재한다면
						i++;
					}
					else{ //해당 파일이 존재하지 않는다면 최초 딜리트 파일!
						rename(delete_file_info.pathname, new_pathname_files); //파일 trash폴더로 이동시킨다.
						fp = fopen(new_pathname_info, "w"); //쓰기모드로 그 파일 연다.
					
						fprintf(fp, "[Trash info]\n");
						fprintf(fp, "%s\n", delete_file_info.pathname); //파일 절대경로 출력
						
						current_time = time(NULL); //현재시간 저장
						localtime_r(&current_time, &tm_time); //현재 시간(삭제시간) tm 구조체로 변환
						fprintf(fp, "D : %04d-%02d-%02d ",tm_time.tm_year+1900, tm_time.tm_mon+1, tm_time.tm_mday);
						fprintf(fp, "%02d:%02d:%02d\n",tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
						
						localtime_r(&delete_file_info.mtime, &tm_time); //삭제할 파일의 최종 수정시간 tm 구조체로 변환
						fprintf(fp, "M : %04d-%02d-%02d ",tm_time.tm_year+1900, tm_time.tm_mon+1, tm_time.tm_mday);
						fprintf(fp, "%02d:%02d:%02d\n",tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);

						fclose(fp);
						break;
					}
				}
				else{ //차후 딜리트 파일 순서대로 검사
					sprintf(new_pathname_files, "%s/%d_%s", "trash/files", i, delete_file_info.fname); //파일에 번호붙여서 새 경로로
					sprintf(new_pathname_info, "%s/%d_%s", "trash/info", i, delete_file_info.fname); //파일에 번호붙여서 새 경로로
					
					if(!access(new_pathname_files, F_OK)){ //해당 파일이 이미 trash/files에 존재한다면
						i++;
					}
					else{ //해당 파일이 존재하지 않는다면 그 번호_파일로 trash에 만들어야 한다.
						rename(delete_file_info.pathname, new_pathname_files); //파일 trash폴더로 이동시킨다.
						fp = fopen(new_pathname_info, "w"); //쓰기모드로 그 파일 연다.
					
						fprintf(fp, "[Trash info]\n");
						fprintf(fp, "%s\n", delete_file_info.pathname); //파일 절대경로 출력
						
						current_time = time(NULL); //현재시간 저장
						localtime_r(&current_time, &tm_time); //현재 시간(삭제시간) tm 구조체로 변환
						fprintf(fp, "D : %04d-%02d-%02d ",tm_time.tm_year+1900, tm_time.tm_mon+1, tm_time.tm_mday);
						fprintf(fp, "%02d:%02d:%02d\n",tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
						
						localtime_r(&delete_file_info.mtime, &tm_time); //삭제할 파일의 최종 수정시간 tm 구조체로 변환
						fprintf(fp, "M : %04d-%02d-%02d ",tm_time.tm_year+1900, tm_time.tm_mon+1, tm_time.tm_mday);
						fprintf(fp, "%02d:%02d:%02d\n",tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);

						fclose(fp);
						break;
					}
				}
			}
			memset(pathname, '\0', MAX_PATH);
			strcpy(pathname, "trash/info");
			index = 0;
			amount = 0;
			while(read_trash_files_info(deleted_file_array, pathname, &amount, &index) >= 2048){ //2키로바이트 넘어섰을 경우
				old_time = time(NULL); //현재시간 초기값으로 넣어주고
				for(i = 0; i < amount; i++){ 
					if(difftime(old_time, deleted_file_array[i].mtime) > 0) { //더 옛날에 삭제된 파일 발견했을 경우
						old_time = deleted_file_array[i].mtime; //old_time 지금걸로 최신화
						old_file_index = i; //old_file_index도 최신화
					}
				}
				rmdirs(deleted_file_array[old_file_index].pathname, 1); //info 디렉토리에 있는 오래된 거 삭제
				sprintf(want_delete_pathname, "%s/%s", "trash/files", deleted_file_array[old_file_index].fname); //file에서 지우고싶은 경로 저장
				rmdirs(want_delete_pathname, 1); //files에서 오래된거 삭제
			}
		}
	}
}
int read_trash_files_info(Fileinfo file_array[ARRAY_MAX], char *pathname, int *amount, int *index){
	struct dirent **dirp;
	struct stat statbuf;
	char *ptr;
	int sum = 0;
	int count;
	int idx;

	ptr = pathname + strlen(pathname);
	*ptr++ = '/';
	*ptr = '\0';
	
	//디렉토리의 파일들 읽는다.
	if((count = scandir(pathname, &dirp, NULL, alphasort)) == -1){
		fprintf(stderr, "Directory scan Error\n");
		exit(1);
	}
	
	//디렉토리 사이즈만큼 반복수행
	for(idx = 0; idx < count; idx++)
	{
		if((!strcmp(dirp[idx]->d_name, ",")) || (!strcmp(dirp[idx]->d_name, ".."))){
			continue;
		}
		lstat(pathname, &statbuf); //파일 정보 받기
		strcpy(file_array[*index].pathname, pathname); //배열에 파일 경로 넣기
		strcpy(file_array[*index].fname, dirp[idx]->d_name); //배열에 파일 이름 넣기
		file_array[*index].mtime = statbuf.st_mtime; //배열에 최종 수정시간 (info안에 파일 만들어진 시간) 저장
		sum += statbuf.st_size; //파일 사이즈 더해줌
		*index++;
		*amount++;
	}

	for (idx = 0; idx < count; idx++) { //할당 해제
		free(dirp[idx]);
	}
	free(dirp); //할당 해제

	ptr[-1] = 0;
	return sum; //info 파일들의 총 사이즈 리턴
}

//path가 파일이면 파일을 삭제하고, path가 디렉토리면 그 하위의 모든 디렉토리와 파일 삭제하는 함수
int rmdirs(const char *path, int force){
	DIR * dir_ptr = NULL;
	struct dirent *file = NULL;
	struct stat buf;
	char filename[1024];

	//디렉토리 오픈, 디렉토리가 아니라면
	if((dir_ptr = opendir(path)) == NULL) {
		return unlink(path); //삭제하고 리턴
	}

	//디렉토리 내의 파일들을 차례대로 읽는다.
	while((file = readdir(dir_ptr)) != NULL) {
		if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {
			continue; //"."과 ".."은 스킵
		}
		
		//디렉토리 내의 파일들 경로 이어준다.
		sprintf(filename, "%s/%s", path, file->d_name);
		
		//파일 정보 받아서 buf에 저장
		if(lstat(filename, &buf) == -1) {
			continue;
		}
		
		//디렉토리일 경우
		if(S_ISDIR(buf.st_mode)){
			if(rmdirs(filename, force) == -1 && !force) { //재귀적으로 rmdirs 호출
				return -1;
			}
		}
		//일반파일이거나 링크파일일 경우
		else if(S_ISREG(buf.st_mode) || S_ISLNK(buf.st_mode)) {
			if(unlink(filename) == -1 && !force) { //파일 삭제
				return -1;
			}
		}
	}

	closedir(dir_ptr); //연 디렉토리 닫는다.

	return rmdir(path); //다 비워진 디렉토리 삭제한다.
}

void do_size(char *command){
}

void do_recover(char *command){
}

int do_tree(char *pathname, int depth){
	struct dirent **dirp;
	struct stat statbuf;
	char *ptr;
	int count;
	int idx;
	int i;

	if(depth++ == 0) //맨 첨일 경우
		printf("%-10s",pathname);

	ptr = pathname + strlen(pathname); //인자로 넘어온 경로의 끝으로 포인터 이동
	*ptr++ = '/'; //끝에 / 붙여준다. (경로 잇기)
	*ptr = '\0';

	if((count = scandir(pathname, &dirp, NULL, alphasort)) == -1) { //디렉토리의 파일들 읽어온다.
		fprintf(stderr, "Directory Scan Error\n");
		exit(1);
	}

	//디렉토리 사이즈만큼 반복수행
	for (idx = 0; idx < count; idx++)
	{
		//'.'과 ".." 파일은 넘어가고
		if((!strcmp(dirp[idx]->d_name, ".")) || (!strcmp(dirp[idx]->d_name, ".."))){
			continue;
		}
		//경로의 끝에 현재 검사하려는 파일 이어준다
		strcpy(ptr, dirp[idx]->d_name);

		if(idx == 2){ //첫 파일일 경우
			printf("━┳━━");
		}
		else if(idx == count-1){ //맨 끝 파일일 경우
			for(i = 1; i < depth; i++){ //중간 줄 끼워주기
				if(i == 1)
					printf("%11s┃", " ");
				else
					printf("%13s┃", " ");
			}

			if(depth == 1){  //파일 없는 줄
				printf("%11s┃\n", " ");
			}
			else{
				printf("%13s┃\n", " ");
			}

			for(i = 1; i < depth; i++) //중간 줄 끼워주기
			{
				if(i == 1)
					printf("%11s┃", " ");
				else
					printf("%13s┃", " ");
			}
			if(depth == 1){ //파일 있는 줄
				printf("%11s┻━━", " ");
			}
			else{
				printf("%13s┻━━", " ");
			}
		} 
		else { //중간일 경우
			for(i = 1; i < depth; i++){ //중간 줄 끼워주기
				if(i == 1)
					printf("%11s┃", " ");
				else
					printf("%13s┃", " ");
			}

			if(depth == 1){ //파일 없는 줄
				printf("%11s┃\n", " ");
			}
			else{
				printf("%13s┃\n", " ");
			}

			for(i = 1; i < depth; i++){ //중간 줄 끼워주기
				if(i == 1)
					printf("%11s┃", " ");
				else
					printf("%13s┃", " ");
			}
			if(depth == 1){ //파일 있는 줄
				printf("%11s┣━━", " ");
			}
			else{
				printf("%13s┣━━", " ");
			}
		}
		printf("%-10s", dirp[idx]->d_name); //파일명 출력
		lstat(pathname, &statbuf); //파일의 정보 받아다가
		if (S_ISDIR(statbuf.st_mode)){ //그 파일이 디렉토리일 경우
			do_tree(pathname, depth); //재귀적으로 그 파일로 다시 트리 만들기
		}
		if(idx != count-1){ //맨 끝이 아닐경우
			printf("\n"); //개행
		}
	}

	for (idx = 0; idx <count; idx++) { //포인터 해제
		free(dirp[idx]);
	}
	free(dirp); //포인터 해제

	ptr[-1] = 0; //마지막 '/'을 널문자로 바꿔주기
	return 0;
}

void do_help(void){
	printf("도움말\n");
}
