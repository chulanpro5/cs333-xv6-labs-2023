#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h" 

int is_space(char chr) {
    return (chr == ' ' || chr == '\t');
}

int main(int argc, char *argv[]) {
    char buf[100], ch;
    char *p = buf;
    char *v[MAXARG];
    int c;
    int spaces = 0;
    int offset = 0;

    // Check if there are no command-line arguments
    if (argc <= 1) {
        exit(1); // Exit with an error code
    }

    // Copy command-line arguments to the v array, excluding the program name
    for (c = 1; c < argc; c++) {
        v[c - 1] = argv[c];
    }

    --c;

    // Read characters from standard input
    while (read(0, &ch, 1) > 0) {
        if (is_space(ch)) {
            spaces++;
            continue;
        }

        if (spaces) {
            // Null-terminate the buf string and store its address in v[c]
            buf[offset++] = 0;
            v[c++] = p;
            p = buf + offset;
            spaces = 0;
        }

        if (ch != '\n') {
            // Append non-blank characters to the buf string
            buf[offset++] = ch;
        } else {
            // Null-terminate the buf string and store its address in v[c]
            v[c++] = p;
            p = buf + offset;
            // Fork a new process
            if (!fork()) {
                exit(exec(v[0], v)); // Execute the command in the child process
            }
            wait(0); // Wait for the child process to finish

            c = argc - 1; // Reset c to the number of arguments
        }
    }

    exit(0);
}
