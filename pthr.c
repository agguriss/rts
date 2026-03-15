#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

pthread_mutex_t calc_m;
int sum = 0;

typedef struct {
    int thread;
    int num;
} Params;

char* policyToString(int policy){
	return (
		policy==SCHED_OTHER?"SCHED_OTHER":
		policy==SCHED_RR?"SCHED_RR":
		policy==SCHED_FIFO?"SCHED_FIFO":
		"SCHED_?"
	);
}

void* calc(void* args) {
    Params* par = (Params*)args;

    printf("thread %d number %d\n", par->thread, par->num);

    pthread_mutex_lock(&calc_m);
    sum += par->num;
    pthread_mutex_unlock(&calc_m);

    return NULL;
}

int main(int argc, char** argv) {
	pid_t pid = getpid();
    printf("[*] running process %d\n", pid);
    int policy = sched_getscheduler(pid);
	printf("[*] using default scheduler %d %s\n", policy , policyToString(policy));

	srand(time(NULL));

    int THREADS = (argc > 1) ? atoi(argv[1]) : 4;
    if (THREADS <= 0) {
        printf("Invalid number of threads\n");
        return 1;
    }
	printf("[*] using %d threads\n",THREADS);

    pthread_t threads[THREADS];
    Params par[THREADS];

    if (pthread_mutex_init(&calc_m, NULL) != 0) {
        printf("pthread_mutex_init failed\n");
        return 1;
    }

	struct sched_param param_fifo;
	pthread_attr_t attr_fifo;
	pthread_t thread_fifo;
	int status = -1;

	memset(&param_fifo,0,sizeof(param_fifo));
	status = pthread_attr_init(&attr_fifo);

	if(status){
		perror("pthread_attr_init error\n");
		return status;
	}

	status = pthread_attr_setschedpolicy(&attr_fifo,SCHED_FIFO);
	if(status){
		perror("pthread_attr_setschedpolicy error\n");
		return status;
	}

	param_fifo.sched_priority = sched_get_priority_max(SCHED_FIFO);
	status = pthread_attr_setschedparam(&attr_fifo,&param_fifo);
	if(status){
		perror("pthread_attr_setschedparam error\n");
		return status;
	}

	status = pthread_attr_setinheritsched(&attr_fifo,PTHREAD_EXPLICIT_SCHED);
	if(status){
		perror("pthread_attr_setinheritsched error\n");
		return status;
	}


    for (int i = 0; i < THREADS; i++) {
        par[i].thread = i;
        par[i].num = 1 + rand() % 9;

        status = pthread_create(&threads[i], &attr_fifo, calc, (void*)&par[i]);
        if (status) {
            perror("pthread_create error\n");
			pthread_mutex_destroy(&calc_m);
            return status;
        }
    }

    for (int i = 0; i < THREADS; i++) {
        status = pthread_join(threads[i], NULL);
        if (status) {
			perror("pthread_join error\n");
            pthread_mutex_destroy(&calc_m);
            return status;
        }
    }

    printf("final sum = %d\n", sum);

    pthread_mutex_destroy(&calc_m);
    return 0;
}
