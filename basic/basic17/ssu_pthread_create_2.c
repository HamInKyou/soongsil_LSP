#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define THREAD_NUM 8

//쓰레드 정보 담는 구조체
struct thread_data {
	int thread_index;
	int sum;
	char *message;
};

void *ssu_printhello(void *arg);

//쓰레드 정보들 담는 배열
struct thread_data thread_data_array[THREAD_NUM];

char *messages[THREAD_NUM];

int main(void)
{
	pthread_t tid[THREAD_NUM];
	int sum;
	int i;

	sum = 0;
	messages[0] = "English: Hello World!";
	messages[1] = "French: Bonjour, le monde!";
	messages[2] = "Spanish: Hola al mundo";
	messages[3] = "Klingon: Nuq neH!";
	messages[4] = "German: Guten Tag, Welt!";
	messages[5] = "Russian: Zdravstvytye, mir!";
	messages[6] = "Japan: Sekai e konnichiwa!";
	messages[7] = "Latin: Orbis, te saluto!";
	
	//쓰레드 8개 생성할 수 있게
	for ( i = 0; i < THREAD_NUM; i++) {
		sum = sum + i;
		thread_data_array[i].thread_index = i;
		thread_data_array[i].sum = sum;
		thread_data_array[i].message = messages[i];
		printf("Creating thread %d\n", i);

		//쓰레드 생성, 쓰레드는 ssu_printhello 실행
		if (pthread_create(&tid[i], NULL, ssu_printhello,
				(void *)&thread_data_array[i]) != 0) {
			fprintf(stderr, "pthread_create error\n");
			exit(1);
		}
	}

	sleep(5); //메인 5초 뒤에 끝냄
	exit(0);
}

void *ssu_printhello(void *arg) {
	struct thread_data *data;
	char *message;
	int thread_index;
	int sum;

	sleep(1); //1초 쉬고
	data = (struct thread_data *)arg; //인자로 준 쓰레드 정보 받음
	thread_index = data -> thread_index;
	sum = data -> sum;
	message = data -> message;
	//쓰레드 정보 출력
	printf("Thread %d: %s Sum=%d\n", thread_index, message, sum);
	return NULL;
}
