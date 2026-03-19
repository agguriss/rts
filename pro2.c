#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

int randDelay(){
	srand(time(NULL));
	return (1+rand()%3);
}


int main(int argc, char **argv) {
	pid_t pid = getpid();
	printf("process: %d\n",pid);
    const int WIDTH = (argc > 1 ? atoi(argv[1]) : 30);

    for (int i = 1; i <= WIDTH; i++) {
        printf("\r[");

        for (int j = 0; j < WIDTH; j++) {
            if (j < i)
                printf("#");
            else
                printf(" ");
        }

        printf("]");
        fflush(stdout);
        sleep(randDelay());
    }

    printf("\n");
    return 0;
}
