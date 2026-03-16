#include <stdio.h>
#include <pthread.h>
#include <sched.h>

#define T 4

void* thread(){
	printf("thread func\n");
	int scheduler = sched_getscheduler(getpid());
	printf("scheduler: %d\n",scheduler);
}


int main(){
	pthread_t thread[T];
	printf("main func\n");
	for(int i=0;i<T;i++){
		pthread_create(&thread[i],NULL,thread,NULL);
	}

	for(int i=0;i<T;i++){
		pthread_join(thread[i],NULL);
	}

	return 0;
}
