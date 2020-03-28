#include<stdio.h>
#include<stdlib.h>
#include<unistd.h> //lseek, read 쓰기 위해
#include<fcntl.h> //open() 쓰기 위해
#include "ssu_employee.h" //emplyee 구조체에 대한 정의가 있는 헤더 참조

int main(int argc, char *argv[])
{
	struct ssu_employee record; //구조체 생성
	int fd;
	int record_num; //몇번 째 record를 알고싶은지 넘버를 저장하는 변수

	if (argc < 2){ //실행 입력이 제대로 잘 들어왔는지
		fprintf(stderr,"Usage : %s file\n", argv[0]);
		exit(1);
	}

	if ((fd = open(argv[1], O_RDONLY)) < 0){ //첫번째 실행 인자로 들어온 파일을 열겠다.(읽기모드)
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}

	while (1){
		printf("Enter record number : "); 
		scanf("%d", &record_num); //몇번 째 record를 알고싶은지 입력(0부터 시작)

		if (record_num < 0) //0미만의 수가 입력됐을 시 종료.
			break;
		

		//off_t는 long타입이기에, long 타입으로 캐스팅, 해당 레코드의 시작지점으로 오프셋을 옮긴다.
		if(lseek(fd, (long)record_num * sizeof(record), 0) < 0) {
			fprintf(stderr, "lseek error");
			exit(1);
		}
		
		//record의 사이즈만큼 파일을 읽어서 record에 저장한다. (buf는 void* 형이기에 캐스팅 해줘야한다)
		if (read(fd, (char *)&record, sizeof(record)) > 0)
			printf("Employee : %s Salary : %d\n", record.name, record.salary);
		else
			printf("Record %d not found\n", record_num);
	}

	close(fd);
	exit(0);
}
