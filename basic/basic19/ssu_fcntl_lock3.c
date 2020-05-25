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
	
	//첫번째 인자로 준 파일을 읽고쓰기 모드로 오픈한다.
	if((fd = open(argv[1], O_RDWR)) == -1) {
		perror(argv[1]);
		exit(1);
	}
	
	//현재 프로세스 id 저장한다.
	pid = getpid();

	//무한 반복
	for(;;){
		printf("\nEnter record number: ");
		scanf("%d", &recnum);

		//입력한게 0 미만이면 반복 그만
		if(recnum < 0)
			break;
		//시작 위치를 지정해준다.
		position = recnum * sizeof(record);
		//파일 락을 걸기 위한 과정
		lock.l_type = F_WRLCK; //읽고 쓰기를 차단시켜준다.
		lock.l_whence = 0;
		lock.l_start = position; //락의 시작 위치는 position 부터
		lock.l_len = sizeof(record); //레코드의 길이만큼
		
		//락을 설정해준다.
		if(fcntl(fd, F_SETLKW, &lock) == -1) {
			perror(argv[1]);
			exit(2);
		}
		//position 위치로 오프셋 이동한다.
		lseek(fd, position, 0);
		//레코드 길이만큼 읽는다.
		if(read(fd, (char*)&record, sizeof(record)) == 0) {
			printf("record %d not found\n", recnum);
			lock.l_type = F_UNLCK;
			fcntl(fd, F_SETLK, &lock);
			continue;
		}
		//레코드 내용 출력
		printf("Employee: %s, salary: %d\n", record.name, record.salary);
		record.pid = pid;
		printf("Enter new salary: ");
		scanf("%d", &record.salary); 
		lseek(fd, position, 0);
		write(fd, (char*)&record, sizeof(record)); //수정한 salary 파일에 적용
		
		//락을 해제시켜준다.
		lock.l_type = F_UNLCK;
		fcntl(fd, F_SETLK, &lock);
	}

	//파일 닫는다.
	close(fd);
}
