#include <sched.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

int main(void) {

    int pol;
    struct sched_param sp;
    struct rlimit new, old, *np;

    pol = sched_getscheduler(getpid());
    if (pol == -1) {
        perror("sched_getscheduler");
        exit(EXIT_FAILURE);
    }

    if (sched_getparam(0, &sp) == -1) {
        perror("sched_getparam");
        exit(EXIT_FAILURE);
    }
            
    printf("%ld: %-5s ", (long)getpid(),
            (pol == SCHED_OTHER) ? "OTHER" :
            (pol == SCHED_RR) ? "RR" :
            (pol == SCHED_FIFO) ? "FIFO" :
    #ifdef SCHED_BATCH              /* Linux-specific */
            (pol == SCHED_BATCH) ? "BATCH" :
    #endif
    #ifdef SCHED_IDLE               /* Linux-specific */
            (pol == SCHED_IDLE) ? "IDLE" :
    #endif
            "???");
    printf("%2d\n", sp.sched_priority);

    if (getrlimit(RLIMIT_NICE, &new) == -1) {
        perror("rlimit");
        exit(EXIT_FAILURE);
    }
    printf("Nice limits: soft %ld, hard %ld\n", new.rlim_cur, new.rlim_max);
    
    new.rlim_cur = (rlim_t) 30;
    new.rlim_max = (rlim_t) 30;
    np=&new;

    if (prlimit(getpid(), RLIMIT_NICE, np, &old) == -1) {
        perror("prlimit-1");
        exit(EXIT_FAILURE);
    }
    printf("Previous limits: soft=%jd; hard=%jd\n", 
        (intmax_t) old.rlim_cur, (intmax_t) old.rlim_max);

           /* Retrieve and display new CPU time limit */
    if (prlimit(getpid(), RLIMIT_NICE, NULL, &old) == -1) {
        perror("prlimit-2");
        exit(EXIT_FAILURE);
    }
    printf("New limits: soft=%jd; hard=%jd\n", 
        (intmax_t) old.rlim_cur, (intmax_t) old.rlim_max);


    printf("Nice value is %d\n", getpriority(PRIO_PROCESS, 0));

	if (setpriority(PRIO_PROCESS, 0, -10) == -1) {
        perror("setpriority");
        exit(EXIT_FAILURE);
    }

    printf("New nice value is %d\n", getpriority(PRIO_PROCESS, 0));

    exit(EXIT_SUCCESS);

}
