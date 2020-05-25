#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define NAMESIZE 50

struct employee{
	char name[NAMESIZE];
	int salary;
	int pid;
};

int main(int argc, char *argv[])
{
	struct flock lock;
	struct employee record;
	int fd, recnum, pid;
	long position;
	char ans[5];
	
	//인자로 입력한 파일 읽고 쓰기 모드로 오픈한다.
	if((fd = open(argv[1], O_RDWR)) == -1) {
		perror(argv[1]);
		exit(1);
	}
	
	pid = getpid(); //현재 프로세스 아이디 저장
	//무한 반복
	for(;;){
		printf("\nEnter record number: ");
		scanf("%d", &recnum); 
		if(recnum < 0)
			break;

		position = recnum * sizeof(record);

		//락을 설정해주기 위한 과정
		lock.l_type = F_RDLCK; //읽기를 차단해준다.
		lock.l_whence = 0;
		lock.l_start = position; //position 위치부터
		lock.l_len = sizeof(record); //레코드 길이만큼

		//락을 설정해준다.
		if(fcntl(fd, F_SETLKW, &lock) == -1) {
			perror(argv[1]);
			exit(2);
		}

		//positin 위치로 옮겨준다.
		lseek(fd, position, 0);
		
		//옮긴 위치부터 레코드 읽는다.
		if(read(fd, (char*)&record, sizeof(record)) == 0) {
			printf("record %d not found\n", recnum);
			lock.l_type = F_UNLCK;
			fcntl(fd, F_SETLK, &lock);
			continue;
		}
		//레코드 내용 출력
		printf("Employee: %s, salary: %d\n", record.name, record.salary);
		printf("Do you want to update salary (y or n)? ");
		scanf("%s", ans);
	
		//레코드 내용 바꾸고 싶지 않다면
		if(ans[0] != 'y'){
			lock.l_type = F_UNLCK;
			fcntl(fd, F_SETLK, &lock);
			continue;
		}
		//락을 변경해준다.
		lock.l_type = F_WRLCK; //읽고 쓰기 차단으로
		if(fcntl(fd, F_SETLKW, &lock) == -1) {
			perror(argv[1]);
			exit(3);
		}
		record.pid = pid;
		printf("Enter new salary: ");
		scanf("%d", &record.salary); //새로 바꿀 salary 입력
		
		//position 위치로 이동하여
		lseek(fd, position, 0);
		//salary 바꾼 레코드 파일에 적용시켜준다.
		write(fd, (char*)&record, sizeof(record));
		
		//락 해제시켜준다.
		lock.l_type = F_UNLCK;
		fcntl(fd, F_SETLK, &lock);
	}
	//파일 닫는다.
	close(fd);
}
