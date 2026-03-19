#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(){
	int status = system("gcc pro2.c -o pro2.out");
	perror("compilation");
	status = system("./pro2.out");
	if(status==-1){
		perror("execution");
		return 1;
	}
	if(WIFEXITED(status))
		printf("pro2.c status: %d\n",WEXITSTATUS(status));

	return 0;
}
