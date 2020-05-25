#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define NAMESIZE 50
#define MAXTRIES 5

struct employee{
	char name[NAMESIZE];
	int salary;
	int pid;
};

int main(int argc, char *argv[])
{
	struct flock lock;
	struct employee record;
	int fd, sum=0, try=0;

	sleep(10); //10초간 대기
	
	//인자로 입력한 파일을 읽고쓰기 모드로 오픈한다.
	if((fd = open(argv[1], O_RDONLY)) == -1) {
		perror(argv[1]);
		exit(1);
	}
	
	//파일을 락 시켜주기 위해 lock 구조체 설정해준다.
	lock.l_type = F_RDLCK; //읽기를 차단시켜준다.
	lock.l_whence = 0;
	lock.l_start = 0L;
	lock.l_len = 0L;

	//파일을 락 시켜준다.
	while(fcntl(fd, F_SETLK, &lock) == -1) {
		if(errno == EACCES) {
			if(try++ < MAXTRIES) {
				sleep(1);
				continue;
			}
			printf("%s busy -- try later\n", argv[1]);
			exit(2);
		}
		perror(argv[1]);
		exit(3);
	}

	sum = 0;
	//레코드를 읽어온다.
	while(read(fd, (char *)&record, sizeof(record)) > 0) {
		//레코드 출력 시켜준다.
		printf("Employee: %s, Salary: %d\n", record.name, record.salary);
		sum += record.salary; //Salary sum에 더해준다.
	}
	printf("\nTotal salary: %d\n", sum); //레코드 다 읽고 sum 출력

	//lock 해제시켜준다.
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &lock);
	close(fd);
}

