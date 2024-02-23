#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]){
	int pip[2];  
	char receive[128];

	pipe(pip);

	if (fork() == 0) {  
		read(pip[0], receive, 4);
		close(pip[0]);

		printf("%d: received %s\n", getpid(), receive);

		write(pip[1], "pong", 4);
		close(pip[1]);
	}
    else {  
		write(pip[1], "ping", 4);
		close(pip[1]);

		read(pip[0], receive, 4);
		close(pip[0]);

		printf("%d: received %s\n", getpid(), receive);
	}
	exit(0);
}