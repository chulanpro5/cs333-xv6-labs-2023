#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void sieve(int in_fd) {
    int prime;
    int out_fd;

    // read first prime
    if (read(in_fd, &prime, sizeof(int)) == 0) {
        exit(0);
    }

    printf("prime %d\n", prime);

    // create new pipe
    int p[2];
    pipe(p);
    out_fd = p[1];

    if (fork() == 0) {
        // child process
        close(p[1]);
        sieve(p[0]); // call sieve with read end
        exit(0);
    } else {
        // parent process
        close(p[0]);

        // filter numbers and pass them to the next process
        int num;
        while (read(in_fd, &num, sizeof(int)) > 0) {
            if (num % prime != 0) {
                write(out_fd, &num, sizeof(int));
            }
        }
        close(out_fd);
        wait(0);
        exit(0);
    }
}

int main() {
    int p[2];
    pipe(p);

    if (fork() == 0) {
        // child process
        close(p[1]);
        sieve(p[0]); // start sieve
        exit(0);
    } else {
        // parent process
        close(p[0]);

        for (int i = 2; i <= 35; i++) {
            write(p[1], &i, sizeof(int));
        }
        close(p[1]);
        wait(0);
        exit(0);
    }
}
