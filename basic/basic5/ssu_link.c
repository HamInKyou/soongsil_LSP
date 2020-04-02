#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(int argc ,char *argv[])
{
	if (argc < 3){
		fprintf(stderr, "usage : %s <file1> <file2>\n",argv[0]);
		exit(1);
	}

	if(link(argv[1],argv[2]) == -1) {//하드링크가 안됐을 경우 에러
		fprintf(stderr, "link error for %s\n", argv[0]);
		exit(1);
	}
	exit(0);
}
