#include <stdio.h>
#include <unistd.h>

int main(void) {
    int prev = 0;

    for (int i = 1; i <= 10; i++) {
        printf("\r");
        for (int k = 0; k < prev; k++)
            printf(" ");
        printf("\r");
        for (int j = 0; j < i; j++)
            printf("#");

        fflush(stdout);
        prev = i;
        sleep(1);
    }

    printf("]\n");
    return 0;
}
