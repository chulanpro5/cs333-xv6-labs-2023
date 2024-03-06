#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

#define MAX_PATH 512
#define DIRSIZ 14

char* fmtname(char *path) {	
    char *p;

    // Find first character after last slash.
    for(p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    return p;
}

void find(char *path, char *targetname) {
    int fd;
    struct dirent de;
    struct stat st;
    char buf[MAX_PATH];

    // if equal name print path
    if(!strcmp(fmtname(path), targetname)){
        printf("%s\n", path);
    }

    // open directory
    if((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // get file info
    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // if not directory, return
    if(st.type != T_DIR){
        close(fd);
        return;
    }

    // copy path to buf
    strcpy(buf, path);
    char *p = buf + strlen(buf);

    // add '/' to the end of buf
    *p++ = '/';

    // read directory entries
    while(read(fd, &de, sizeof(de)) == sizeof(de)) {
        if(de.inum == 0) // not have file in this dir
            continue;
        memmove(p, de.name, DIRSIZ); // copy name to buf
        p[DIRSIZ] = 0; // add '\0' to the end of buf

        // ignore '.' and '..'
        if(!strcmp(de.name, ".") || !strcmp(de.name, ".."))
            continue;

        find(buf, targetname); // recursive call find
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if(argc < 3){
        fprintf(2, "Usage: find <path> <targetname>\n");
        exit(1);
    }

    find(argv[1], argv[2]);

    exit(0);
}