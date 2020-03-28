#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

#define TABLE_SIZE 128
#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
	static struct {
		long offset; //시작지점 오프셋
		int length; //길이
	} table[TABLE_SIZE]; //구조체 배열 128칸짜리 생성
	char buf[BUFFER_SIZE];
	long offset;
	int entry;
	int i;
	int length;
	int fd;

	if (argc < 2) { //실행 입력 제대로 입력했는지
		fprintf(stderr, "usage: %s <file>\n", argv[0]);
		exit(1);
	}

	if((fd = open(argv[1], O_RDONLY)) < 0) { //인자로 들어온 파일 읽기모드로 오픈
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}

	entry = 0;
	offset = 0;

	//버퍼사이즈인 1024바이트씩 파일에서 읽어서 buf에 집어넣는다.
	while ((length = read(fd, buf, BUFFER_SIZE)) > 0) {
		//읽은 만큼 반복하는데, 반복할 때마다 구조체 한칸의 length를 하나 늘림
		for(i = 0; i < length; i++) {
			table[entry].length++;
			offset++; //개행 넘어가고 다음 오프셋이 다음 문장 시작지점이 되게

			if (buf[i] == '\n') //개행문자 받았을 경우, 다음 구조체의 offset을 지정한 오프셋으로 저장
				table[++entry].offset = offset;
		}
	}

//DEBUG용, 해당 행 시작지점의 offset과 길이 정보를 출력함
#ifdef DEBUG
	for ( i = 0; i < entry; i++)
		printf("%d : %ld, %d\n", i + 1, table[i].offset, table[i].length);
#endif
	
	//여기서의 length는 원하고자 하는 라인의 넘버일뿐, 위의 length랑은 다른 의미
	//원하고자 하는 라인 넘버 입력받아서 그 라인을 출력
	while (1) {
		printf("Enter line number : ");
		scanf("%d", &length);

		if (--length < 0) //0번째 줄 미만으로 입력 받았을때 중지
			break;
		
		lseek(fd, table[length].offset, 0); //해당 라인의 시작점 위치로 오프셋 옮긴다.

		if (read(fd, buf, table[length].length) <= 0) //해당 라인의 길이만큼 읽어서 buf에 넣는다.
			continue;

		buf[table[length].length] = '\0'; //끝에 널문자 넣는다.
		printf("%s", buf);
	}

	close(fd);
	exit(0);

}
