#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[]){
    if(argc<=1){
        fprintf(2, "usage:sleep {ticks}");
        exit(1);
    }
    int num_ticks = atoi(argv[1]);
    sleep(num_ticks);
    exit(0);
}
