#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(void)
{
	char path_resolved[1000];
	
	realpath("a.c", path_resolved);

	printf("%s\n", path_resolved);
}
