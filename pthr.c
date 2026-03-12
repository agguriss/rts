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
	sleep(1);
    int policy = sched_getscheduler(pid);
	printf("[*] using default scheduler %d %s\n", policy , policyToString(policy));
	sleep(1);

	srand(time(NULL));

    int THREADS = (argc > 1) ? atoi(argv[1]) : 4;
    if (THREADS <= 0) {
        printf("Invalid number of threads\n");
        return 1;
    }
	printf("[*] using %d threads\n",THREADS);
	sleep(1);

    pthread_t threads[THREADS];
    Params par[THREADS];

    if (pthread_mutex_init(&calc_m, NULL) != 0) {
        printf("pthread_mutex_init failed\n");
        return 1;
    }

    for (int i = 0; i < THREADS; i++) {
        par[i].thread = i;
        par[i].num = 1 + rand() % 9;

        int rc = pthread_create(&threads[i], NULL, calc, (void*)&par[i]);
        if (rc != 0) {
            printf("pthread_create error: %s\n", strerror(rc));
            pthread_mutex_destroy(&calc_m);
            return 1;
        }
    }

    for (int i = 0; i < THREADS; i++) {
        int rc = pthread_join(threads[i], NULL);
        if (rc != 0) {
            printf("pthread_join error: %s\n", strerror(rc));
            pthread_mutex_destroy(&calc_m);
            return 1;
        }
    }

    printf("final sum = %d\n", sum);

    pthread_mutex_destroy(&calc_m);
    return 0;
}
