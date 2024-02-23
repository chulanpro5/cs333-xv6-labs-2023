#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char* fmtname(char *path){
	char *p;

    p = path+strlen(path);
    while (p >= path && *p != '/') p--;
	p++;

	return p;
}

void find(char *path, char *targetname){
	char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;

	if (!strcmp(fmtname(path), targetname)) {
		printf("%s\n", path);
	}

	if ((fd = open(path, O_RDONLY)) < 0) {
		fprintf(2, "find: cannot open [%s], fd=%d\n", path, fd);
		return;
	}

	if (fstat(fd, &st) < 0) {
		fprintf(2, "find: cannot stat %s\n", path);
		close(fd);
		return;
	}

	switch(st.type){
  	case T_DEVICE:
  	case T_FILE:
		printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
		break;

  	case T_DIR:
		if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
			printf("ls: path too long\n");
			break;
		}
		strcpy(buf, path);
		p = buf+strlen(buf);
		*p++ = '/';
		while(read(fd, &de, sizeof(de)) == sizeof(de)){
			if(de.inum == 0)
				continue;
			memmove(p, de.name, DIRSIZ);
			p[DIRSIZ] = 0;
			if(stat(buf, &st) < 0){
				printf("ls: cannot stat %s\n", buf);
				continue;
			}
			if (!strcmp(de.name, ".") || !strcmp(de.name, ".."))
				continue;
			find(buf, targetname);
		}
		break;
	}
	close(fd);
}

int main(int argc, char *argv[]){
	if(argc < 3){
		fprintf(2, "usage: find path filename\n");
		exit(1);
	}

	find(argv[1], argv[2]);

	exit(0);
}