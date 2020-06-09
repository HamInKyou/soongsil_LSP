#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <utime.h>
#include <dirent.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

#define BUFFER_SIZE 1024
#define PATH_SIZE 128
#define FILE_AMOUNT 100

typedef struct f_info{
	char fname[BUFFER_SIZE];
	int fsize;
} Fileinfo;

//함수들
void printUsage();
void canAccess();
void do_rsync();
void create_rsync_file();
void create_rsync_dir();
static void signal_handler(int signo);

//전역변수들
Fileinfo rsync_files_info[FILE_AMOUNT];

char src[PATH_SIZE] = {'\0',};
char dst[PATH_SIZE] = {'\0',};

char pathname[PATH_SIZE] = {'\0',};
char filename[PATH_SIZE] = {'\0',};
char tmppathname[PATH_SIZE] = {'\0',};

int arraySize = 0;

int rOption = 0;
int tOption = 0;
int mOption = 0;

int main(int argc, char *argv[])
{	
	FILE *fp;
   struct timeval startTime, endTime;
   double diffTime;

	time_t currentTime;
	char timeStr[25] = {'\0',};
	int c;
	int i;

 	gettimeofday(&startTime, NULL);

	
	//옵션 인자 받는다.
	c = getopt(argc, argv, "rtm");
	switch(c){
		case 'r':
			rOption = 1;
			break;
		case 't':
			tOption = 1;
			break;
		case 'm':
			mOption = 1;
			break;
		case '?':
			fprintf(stderr,"Unknown option error\n");
			exit(1);
	}
	
	//모든 옵션 꺼져 있을 경우 인자 제대로 들어왔는지 체크
	if(rOption == 0 && tOption == 0 && mOption == 0){
		if(argc != 3){
			printUsage();
			exit(1);
		}
		realpath(argv[1], src); //src 절대경로로 바꿔주기
		realpath(argv[2], dst); //dst 절대경로로 바꿔주기
	}
	else{//옵션 켜져있는 경우 인자 제대로 들어왔는지 체크
		if(argc != 4){
			printUsage();
			exit(1);
		}

		fprintf(stderr, "I can't handle options\n"); //옵션 못다룬다하고
		exit(1); //끝내기

		realpath(argv[2], src); //src 절대경로로 바꿔주기
		realpath(argv[3], dst); //dst 절대경로로 바꿔주기
	}

	canAccess(); //src, dst 접근 가능한지?
	do_rsync(); //동기화 실행

	time(&currentTime); //현재 시간 받기
	strncpy(timeStr, ctime(&currentTime), strlen(ctime(&currentTime))-1); //문자열 형태로 만들어주기
	fp = fopen("ssu_rsync_log", "a");
	fprintf(fp, " [%s] %s %s %s\n", timeStr, argv[0], argv[1], argv[2]); //기록 남기기
	for(i = 0; i <arraySize; i++){
		fprintf(fp, "         %s %dbytes\n", rsync_files_info[i].fname, rsync_files_info[i].fsize);
	}
	fclose(fp);
 
 	gettimeofday(&endTime, NULL);
   diffTime = (endTime.tv_sec - startTime.tv_sec) + ((endTime.tv_usec - startTime.tv_usec)/1000000);
   printf("소요시간 : %f sec\n", diffTime);

}

//Usage 출력
void printUsage(){
	fprintf(stderr, "<<Usage>>\n");
	fprintf(stderr, "./ssu_rsync [option] <src> <dst>\n");
	fprintf(stderr, "[option] : r, t, m 하나만 가능 (옵션 구현x)\n");
	fprintf(stderr, "<src> : 동기화 할 파일 혹은 디렉토리\n");
	fprintf(stderr, "<dst> : 동기화해서 <src> 저장할 디렉토리\n");
}

//src, dst 존재하는지? 접근 가능하는지? 판단
//접근 불가능할 경우 Usage 출력하고 프로그램 끝냄
void canAccess(){
	struct stat statbuf;

	if(access(src, F_OK) != 0){ //src가 존재하는지?
		printUsage();
		exit(1);
	}
	if(access(src, R_OK) != 0){ //읽기 가능한지?
		printUsage();
		exit(1);
	}
	if(access(dst, F_OK) != 0){ //dst가 존재하는지?
		printUsage();
		exit(1);
	}
	stat(dst, &statbuf);
	if(!S_ISDIR(statbuf.st_mode)){ //dst가 디렉토리인지?
		printUsage();
		exit(1);
	}
	if(access(dst, R_OK) != 0){ //읽기 가능한지?
		printUsage();
		exit(1);
	}
}

void do_rsync(){
	struct stat srcstatbuf;
	struct stat dststatbuf;
	char *ptr;

	ptr = rindex(src, '/') + 1; //src 파일이름 시작되는 곳
	strcpy(filename, ptr); //파일 이름 filename에 복사
	
	stat(src, &srcstatbuf); //src 파일정보 받아오기

	signal(SIGINT, signal_handler); //시그널 핸들러 등록

	if(S_ISDIR(srcstatbuf.st_mode)){ //src가 디렉토리일 경우
		create_rsync_dir(); //동기화 진행
	}
	else{ //src가 디렉토리가 아닐 경우
		sprintf(pathname, "%s/%s", dst, filename); //dst에 들어갈 파일 경로 받기
		if(access(pathname, F_OK) == 0){ //dst에 동기화된 파일이 이미 있을 경우
			stat(pathname, &dststatbuf); //dst에 동기화된 파일의 정보 받는다.
			if(srcstatbuf.st_mtime != dststatbuf.st_mtime){ //최종 수정시간 다를경우
				sprintf(tmppathname, "%s/tmp_%s", dst, filename); //임시 파일 이름 만들기
				rename(pathname, tmppathname); //임시 파일로 저장
				create_rsync_file(); //새로 dst에 파일 만들어줌
				unlink(tmppathname); //임시파일 지워준다.
			}
		}
		else{ //dst에 동기화된 파일이 없을 경우
			create_rsync_file();
		}
	}
}

void create_rsync_file(){
	FILE *srcfp;
	FILE *dstfp;
	char *ptr;
	struct stat srcstatbuf;
	struct stat dststatbuf;
	struct utimbuf time_buf;
	int c;

	srcfp = fopen(src, "r"); 
	dstfp = fopen(pathname, "w");
	
	while(1)
	{
		c = fgetc(srcfp);

		if(!feof(srcfp))
			fputc(c, dstfp);
		else
			break;
	}

	fclose(srcfp);
	fclose(dstfp);

	stat(src, &srcstatbuf);
	time_buf.actime = srcstatbuf.st_atime;
	time_buf.modtime = srcstatbuf.st_mtime;

	utime(pathname, &time_buf);

	ptr = rindex(src, '/') + 1; //src 파일이름 시작되는 곳

	strcpy(rsync_files_info[arraySize].fname, ptr); //파일 이름 rsync_files_info 구조체에 저장
	rsync_files_info[arraySize].fsize = srcstatbuf.st_size; //파일 사이즈도 저장
	arraySize++; //rsync_files_info 구조체에 저장된 파일 수 1 증가
}

void create_rsync_dir(){
	struct dirent **dirp;
	struct stat statbuf;
	struct stat dststatbuf;
	char *srcptr;
	char *dstptr;
	char fname[PATH_SIZE];
	int count;
	int idx;
	
	srcptr = src + strlen(src); //src 경로의 끝으로 포인터 이동
	*srcptr++ = '/'; //끝에 / 붙여준다. (경로잇기)
	*srcptr = '\0';

	strcpy(pathname, dst); //pathname에 dst까지 경로 복사한다.
	dstptr = pathname + strlen(pathname); //pathname 경로의 끝으로 포인터 이동
	*dstptr++ = '/';
	*dstptr = '\0';

	count = scandir(src, &dirp, NULL, alphasort); //디렉토리의 파일들 읽어온다.
	
	//디렉토리 사이즈만큼 반복수행
	for(idx = 0; idx < count; idx++){
		//'.'과 ".." 파일은 넘어가고
		if((!strcmp(dirp[idx]->d_name, ".")) || (!strcmp(dirp[idx]->d_name, "..")))
			continue;
		
		//경로의 끝에 현재 검사하려는 파일 이어준다.
		strcpy(srcptr, dirp[idx]->d_name);
		stat(src, &statbuf); //src 정보 받아온다.

		if(S_ISDIR(statbuf.st_mode)) //그 파일이 디렉토리이면
			continue; //넘어간다.
		
		//경로의 끝에 동기화해서 두고싶은 파일의 경로를 이어준다.
		strcpy(dstptr, dirp[idx]->d_name);
		if(access(pathname, F_OK) == 0){ //dst에 이미 동기화된 파일 존재할 경우
			stat(pathname, &dststatbuf); //그 파일의 정보 저장
			if(statbuf.st_mtime == dststatbuf.st_mtime){ //최종 수정시간 같을 경우
				continue; 
			}
		}
		strcpy(fname, dirp[idx]->d_name);
		sprintf(tmppathname, "%s/tmp_%s", dst, fname); //임시 파일 이름 만들기
		rename(pathname, tmppathname); //임시 파일로 저장
		create_rsync_file(); //파일 동기화 해준다.
		unlink(tmppathname); //임시파일 지워준다.
	}

	for(idx = 0; idx <count; idx++) //할당해제
		free(dirp[idx]);
	free(dirp); //할당해제

	srcptr[-1] = 0;
	dstptr[-1] = 0;
}

//SIGINT 처리하기 위한 함수
static void signal_handler(int signo){
	struct dirent **dirp;
	char fname[PATH_SIZE];
	char tmpfname[PATH_SIZE];
	char tmppathname[PATH_SIZE];
	char *ptr;
	int idx;
	int count;

	strcpy(pathname, dst); //pathname에 dst까지 경로 복사한다.
	ptr = pathname + strlen(pathname); //pathname 경로의 끝으로 포인터 이동
	*ptr++ = '/';
	*ptr = '\0';
	
	count = scandir(pathname, &dirp, NULL, alphasort);

	for(idx = 0; idx < count; idx++){
		if((!strcmp(dirp[idx]->d_name, ".")) || (!strcmp(dirp[idx]->d_name, "..")))
			continue;
		
		strcpy(fname, dirp[idx]->d_name); //파일 명 fname에 복사
		if(!strncmp(fname, "tmp_", 4)){ //tmp파일일 경우
			continue; //패스
		}
		sprintf(tmpfname, "tmp_%s", fname); //임시파일명 만들기

		strcpy(ptr, tmpfname);
		strcpy(tmppathname, pathname);// 경로로 바꾸기
		if(access(tmppathname, F_OK)==0){ //임시 파일이 존재한다면
			strcpy(ptr, dirp[idx]->d_name); //지금 검사하는 파일
			unlink(pathname); //지금 검사하는 파일 지운다.
			rename(tmppathname,pathname); //임시 파일 원래대로 돌려놓는다.
		}
	}
	for(idx = 0; idx < count; idx++)
		free(dirp[idx]);
	free(dirp);

	exit(1);
}
