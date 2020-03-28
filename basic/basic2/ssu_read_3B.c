#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

int main(void)
{
	char character;
	int fd;
	int line_count = 0;

	if ((fd = open("ssu_test.txt", O_RDONLY)) < 0) { //읽기 모드로 오픈
		fprintf(stderr, "open error for %s\n", "ssu_test.txt");
		exit(1);
	}

	//파일을 한글자씩 읽는데, 개행 문자를 읽었을 경우 line_count를 하나 늘려준다.
	while (1) {
		if (read(fd, &character, 1) > 0) {
			if(character == '\n')
				line_count++;
		}
		else
			break;
	}

	printf("Total line : %d\n", line_count);
	exit(0);
}
