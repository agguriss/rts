#include <stdio.h>
#include <sched.h>
#include <string.h>
#include <unistd.h>

char* getSchedStr(int s){
		char* str = 
		s==SCHED_OTHER?"SCHED_OTHER":
		s==SCHED_RR?"SCHED_RR":
		s==SCHED_FIFO?"SCHED_FIFO":
		"LINUX SPECIFIC";
	return str;
}


int main(){
	pid_t pid = getpid();
	int s = sched_getscheduler(pid);
	struct sched_param par;
	memset(&par,0,sizeof(par));
	par.sched_priority = 99;
	printf("process %ld, scheduler %d-%s\n",pid,s,getSchedStr(s));
	if(sched_setscheduler(pid,SCHED_RR,&par))
		perror("setschedule error\n");
	return 0;
}
