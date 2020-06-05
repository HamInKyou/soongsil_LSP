#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <utime.h>
#include <dirent.h>

#define BUFFER_SIZE 1024
#define PATH_SIZE 128

//함수들
void printUsage();
void canAccess(char *src, char *dst);
void do_rsync(char *src, char *dst);
void create_rsync_file(char *src, char *pathname);
int check_dir_modifies(char *src, char *pathname);
int rmdirs(const char *path, int force);
void create_rsync_dir(char *src, char *pathname);

//전역변수들
int rOption = 0;
int tOption = 0;
int mOption = 0;

int main(int argc, char *argv[])
{	
	char src[PATH_SIZE];
	char dst[PATH_SIZE];
	int c;

	
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
		realpath(argv[2], src); //src 절대경로로 바꿔주기
		realpath(argv[3], dst); //dst 절대경로로 바꿔주기
	}

	canAccess(src, dst); //src, dst 접근 가능한지?
	do_rsync(src, dst); //동기화 실행
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
void canAccess(char *src, char *dst){
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
void do_rsync(char *src, char *dst){
	char pathname[PATH_SIZE];
	char filename[PATH_SIZE];
	char tmppathname[PATH_SIZE];
	struct stat srcstatbuf;
	struct stat dststatbuf;
	char *ptr;

	ptr = rindex(src, '/') + 1; //src 파일이름 시작되는 곳
	strcpy(filename, ptr); //파일 이름 filename에 복사
	sprintf(pathname, "%s/%s", dst, filename); //dst에 들어갈 파일 경로
	
	stat(src, &srcstatbuf); //src 파일정보 받아오기

	if(S_ISDIR(srcstatbuf.st_mode)){ //src가 디렉토리일 경우
		if(access(pathname, F_OK) == 0){ //dst에 동기화된 디렉토리가 이미 있을 경우
			if(check_dir_modifies(src, pathname)){ //최종 수정시간 다른 파일이 디렉토리에 있을 경우
				sprintf(tmppathname, "%s/tmp_%s", dst, filename); //임시 디렉토리 이름 만들기
				rename(pathname, tmppathname); //임시 디렉토리로 저장
				create_rsync_dir(src, pathname); //동기화 진행
				rmdirs(tmppathname, 1); //임시로 만든거 지우기
			}
		}
		else{ //dst에 동기화된 디렉토리가 없을 경우
			create_rsync_dir(src, pathname); //동기화 진행
		}
	}
	else{ //src가 디렉토리가 아닐 경우
		if(access(pathname, F_OK) == 0){ //dst에 동기화된 파일이 이미 있을 경우
			stat(pathname, &dststatbuf); //dst에 동기화된 파일의 정보 받는다.
			if(srcstatbuf.st_mtime != dststatbuf.st_mtime){ //최종 수정시간 다를경우
				sprintf(tmppathname, "%s/tmp_%s", dst, filename); //임시 파일 이름 만들기
				rename(pathname, tmppathname); //임시 파일로 저장
				create_rsync_file(src, pathname); //새로 dst에 파일 만들어줌
				unlink(tmppathname); //임시파일 지워준다.
			}
		}
		else{ //dst에 동기화된 파일이 없을 경우
			create_rsync_file(src, pathname);
		}
	}
}

void create_rsync_file(char *src, char *pathname){
	FILE *srcfp;
	FILE *dstfp;
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
}

int check_dir_modifies(char *src, char *pathname){
	struct dirent **dirp1;
	struct dirent **dirp2;
	struct stat srcstatbuf;
	struct stat dststatbuf;
	char *srcptr;
	char *dstptr;
	int count1;
	int count2;
	int idx;
	
	srcptr = src + strlen(src); //src 경로의 끝으로 포인터 이동
	*srcptr++ = '/'; //끝에 / 붙여준다. (경로잇기)
	*srcptr = '\0';

	dstptr = pathname + strlen(pathname); //pathname 경로의 끝으로 포인터 이동
	*dstptr++ = '/';
	*dstptr = '\0';

	count1 = scandir(src, &dirp1, NULL, alphasort); //src 디렉토리의 파일들 읽어온다.
	count2 = scandir(pathname, &dirp2, NULL, alphasort); //dst에 있는 동기화된 디렉토리의 파일들 읽어온다.
	
	if(count1 != count2){ //디렉토리 안의 파일 수 다를 경우 -> 수정이 일어났다.
		srcptr[-1] = 0;
		dstptr[-1] = 0;
		return 1;
	}
	//디렉토리 사이즈만큼 반복수행
	for(idx = 0; idx < count1; idx++){
		//'.'과 ".." 파일은 넘어가고
		if((!strcmp(dirp1[idx]->d_name, ".")) || (!strcmp(dirp1[idx]->d_name, "..")))
			continue;
		
		//경로의 끝에 현재 검사하려는 파일 이어준다.
		strcpy(srcptr, dirp1[idx]->d_name);
		stat(src, &srcstatbuf); //src 정보 받아오기

		if(S_ISDIR(srcstatbuf.st_mode)) //그 파일이 디렉토리이면
			continue; //넘어간다.
		
		//경로의 끝에 동기화된 파일의 경로를 이어준다.
		strcpy(dstptr, dirp1[idx]->d_name);
		stat(pathname, &dststatbuf); //dst에 있는 동기화된 파일 정보 받아오기

		if(srcstatbuf.st_mtime != dststatbuf.st_mtime){ //수정 시간이 바뀌었을 경우
			srcptr[-1] = 0;
			dstptr[-1] = 0;
			return 1;
		}
	}

	for(idx = 0; idx <count1; idx++) //할당해제
		free(dirp1[idx]);
	free(dirp1); //할당해제
	
	for(idx = 0; idx <count2; idx++) //할당해제
		free(dirp2[idx]);
	free(dirp2); //할당해제

	srcptr[-1] = 0;
	dstptr[-1] = 0;
	return 0;
}

//디렉토리 삭제해주는 함수
int rmdirs(const char *path, int force){
	DIR *dir_ptr = NULL;
	struct dirent *file = NULL;
	struct stat buf;
	char fname[1024];
	
	//디렉토리 오픈, 디렉토리가 아니라면
	if((dir_ptr = opendir(path)) == NULL) {
		return unlink(path); //삭제하고 리턴
	}
	//디렉토리 내의 파일들을 차례대로 읽는다.
	while((file = readdir(dir_ptr)) != NULL){
		if(!strcmp(file->d_name, ".") || !strcmp(file->d_name, ".."))
			continue;
		
		//디렉토리 내의 파일들 경로 이어준다.
		sprintf(fname, "%s/%s", path,file->d_name);
		
		//파일 정보 받아서 buf에 저장
		if(lstat(fname, &buf) == -1){
			continue;
		}
		//디렉토리일 경우
		if(S_ISDIR(buf.st_mode)){
			if(rmdirs(fname, force) == -1 && !force)
				return -1;
		}
		//일반파일이거나 링크파일일 경우
		else if(S_ISREG(buf.st_mode) || S_ISLNK(buf.st_mode)){
			if(unlink(fname) == -1 && !force) //파일 삭제
				return -1;
		}
	}
	closedir(dir_ptr); //연 디렉토리 닫는다.

	return rmdir(path); //다 비워진 디렉토리 삭제한다.
}


void create_rsync_dir(char *src, char *pathname){
	struct dirent **dirp;
	struct stat statbuf;
	char *srcptr;
	char *dstptr;
	int count;
	int idx;
	
	srcptr = src + strlen(src); //src 경로의 끝으로 포인터 이동
	*srcptr++ = '/'; //끝에 / 붙여준다. (경로잇기)
	*srcptr = '\0';

	mkdir(pathname, 0777);
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
		//파일 동기화해준다.
		create_rsync_file(src, pathname);
	}

	for(idx = 0; idx <count; idx++) //할당해제
		free(dirp[idx]);
	free(dirp); //할당해제

	srcptr[-1] = 0;
	dstptr[-1] = 0;
}
