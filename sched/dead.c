#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <linux/sched.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>

struct sched_attr{
	uint32_t size;
	uint32_t sched_policy;
	uint64_t sched_flags;
	int32_t sched_nice;
	uint32_t sched_priority;
	uint64_t sched_runtime;
	uint64_t sched_deadline;
	uint64_t sched_period;
}

long sched_setattr(pid_t pid,const struct sched_attr* attr,unsigned int flags){
	return syscall(__NR_sched_setattr,pid,attr,flags);
}

void* threadA(){
	long rtv;
	struct sched_attr attr={
		.size = sizeof(attr),
		.sched_policy = SCHED_DEADLINE,
		.sched_runtime = (uint64_t)10*1000*1000;
		.sched_period = (uint64_t)2*1000*1000*1000;
		.sched_deadline = (uint64_t)11*1000*1000;
	};
	rtv = sched_setattr(0,&attr,0);
	if(rtv==-1){
		perror("sched_setattr");
		exit(EXIT_FAILURE);
	}
	for(;;){
		printf("sensor\n");
		fflush(0);
		sched_yield();
	};
}


int main(){
	pthread_t pthreadA;
	pthread_create(&pthreadA,NULL,threadA,NULL);
	pthread_exit(0);
	return(EXIT_SUCCESS);
}	
