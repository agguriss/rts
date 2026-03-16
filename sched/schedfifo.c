#include <pthread.h>
#include <stdio.h>
#include <string.h>

void *thread_function_fifo(void* data){
	printf("thread function\n");
	return NULL;
}

int main(int argc,char** argv){
	struct sched_param param_fifo;
	pthread_attr_t attr_fifo;
	pthread_t thread_fifo;
	int status = -1;
	memset(&param_fifo,0,sizeof(param_fifo));
	if(status){
		printf("pthread_attr_init failed\n");
		return NULL;
	}
	if(status){
		printf("pthread_attr_setschedpolicy failed\n");
		return NULL;
	}
	param.sched_priority = sched_get_priority_max(SCHED_FIFO)

	return 0;
}
