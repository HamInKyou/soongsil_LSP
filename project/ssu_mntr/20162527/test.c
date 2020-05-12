#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(void)
{
	char path_resolved[1000];
	char *ptr;
	fgets(path_resolved, 1000, stdin);
	ptr = path_resolved + strlen(path_resolved);
	printf("%d\n", strlen(path_resolved));
	*(ptr-1) = '\0';
	printf("%s", path_resolved);
}
