#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <linux/sched.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>

#define THREAD_NUM 2

typedef struct{
	int arg;
	int ret;
}Data;

long my_sched_setattr(pid_t pid,const struct sched_attr* attr,unsigned int flags){
	return syscall(__NR_sched_setattr,pid,attr,flags);
}

void* thread(void* data){
	Data* datal = (Data*)data;
	long rtv;
	struct sched_attr attr = {
		.size = sizeof(attr),
		.sched_policy = SCHED_DEADLINE,
		.sched_runtime = (uint64_t)10*1000*1000,
		.sched_period = (uint64_t)1*1000*1000*1000,
		.sched_deadline = (uint64_t)11*1000*1000,
	};

	rtv = my_sched_setattr(0,&attr,0);
	if(rtv==-1){
		perror("sched_setattr");
		exit(EXIT_FAILURE);
	}
	for(;;){
		printf("sensor %d\n",datal->arg);
		fflush(stdout);
		sched_yield();
	}
}

int main(){
	pthread_t pthread[THREAD_NUM];
	Data data[THREAD_NUM];
	for(int i=0;i<THREAD_NUM;i++){
		data[i].arg = i;
		data[i].ret = 0;
		pthread_create(&pthread[i],NULL,thread,(void*)&data[i]);
	}
	pthread_exit(0);
	return EXIT_SUCCESS;
}


