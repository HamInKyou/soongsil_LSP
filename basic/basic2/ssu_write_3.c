#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "ssu_employee.h" //구조체 정보 갖고오기

int main(int argc, char *argv[])
{
	struct ssu_employee record; //구조체 생성
	int fd;

	//실행 입력이 잘못됐을 경우 에러메세지 출력
	if (argc < 2) {
		fprintf(stderr, "usage : %s file\n", argv[0]);
		exit(1);
	}

	//첫번째 인자를 쓰기모드로 오픈하는데, 이미 존재할경우 에러!
	//유저에게 읽기와 쓰기, 그룹에게 읽기 권한을 부여
	if ((fd = open(argv[1], O_WRONLY|O_CREAT|O_EXCL, 0640)) < 0) {
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}
	
	//employee 구조체의 정보를 반복해서 입력받아 파일에 쓴다.
	while (1) {
		printf("Enter employee name <SPACE> salary: ");
		scanf("%s", record.name); //이름을 우선 입력받는다.

		//.일 경우 더이상 입력을 안받는다 판단하고 끝낸다.
		if (record.name[0] == '.')
			break;

		//봉급 정보도 입력 받는다.
		scanf("%d", &record.salary);
		record.pid = getpid();
		//입력받은 구조체 정보를 파일에 쓴다.
		write(fd, (char *)&record, sizeof(record));
	}

	close(fd);
	exit(0);
}
