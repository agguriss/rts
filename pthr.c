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
    unsigned int seed = time(NULL) ^ (par->thread * 12345u);

    while (1) {
        int num = -15 + rand_r(&seed) % 25;

        pthread_mutex_lock(&calc_m);

        if (sum < 0) {
            pthread_mutex_unlock(&calc_m);
    	    printf("thread %d hit a negative (%d)\n", par->thread, sum);
            exit(EXIT_FAILURE);
        }

        sum += num;
        printf("thread %d number %d, sum = %d\n", par->thread, num, sum);

        if (sum < 0) {
            printf("thread %d hit a negative (%d)\n", par->thread, sum);
            exit(EXIT_FAILURE);
	    }

        pthread_mutex_unlock(&calc_m);

        sleep(1 + rand_r(&seed) % 3);
    }

    return NULL;
}
int main(int argc, char** argv) {
	pid_t pid = getpid();
    printf("[*] running process %d\n", pid);
    int policy = sched_getscheduler(pid);
	printf("[*] using default scheduler %d %s\n", policy , policyToString(policy));

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
	int status = -1;

	memset(&param_fifo,0,sizeof(param_fifo));
	status = pthread_attr_init(&attr_fifo);

	if(status){
		printf("pthread_attr_init error: %s\n", strerror(status));
		return status;
	}
	policy = SCHED_RR;
	status = pthread_attr_setschedpolicy(&attr_fifo,policy);
	if(status){
		printf("pthread_attr_setschedpolicy error: %s\n", strerror(status));
		return status;
	}

	param_fifo.sched_priority = sched_get_priority_max(policy);
	status = pthread_attr_setschedparam(&attr_fifo,&param_fifo);
	if(status){
		printf("pthread_attr_setschedparam error: %s\n", strerror(status));
		return status;
	}

	status = pthread_attr_setinheritsched(&attr_fifo,PTHREAD_EXPLICIT_SCHED);
	if(status){
		printf("pthread_attr_setinheritsched error: %s\n", strerror(status));
		return status;
	}
	printf("[*] running proccess with scheduler: %s\n",policyToString(policy));

    for (int i = 0; i < THREADS; i++) {
        par[i].thread = i;

        status = pthread_create(&threads[i], &attr_fifo, calc, (void*)&par[i]);
        if (status) {
			printf("pthread_create error: %s\n", strerror(status));
			pthread_mutex_destroy(&calc_m);
            return status;
        }
    }

    for (int i = 0; i < THREADS; i++) {
        status = pthread_join(threads[i], NULL);
        if (status) {
			printf("pthread_join error: %s\n", strerror(status));
            pthread_mutex_destroy(&calc_m);
            return status;
        }
    }

    printf("final sum = %d\n", sum);

    pthread_mutex_destroy(&calc_m);
    return 0;
}
