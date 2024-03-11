#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

// Get from ls.c
char* fmtname(char *path) {	
	char *p;

	// Find first character after last slash.
	for(p=path+strlen(path); p >= path && *p != '/'; p--)
		;
	p++;

	return p;
}

void find(char *path, char *targetname) {	
	int fd;
	struct dirent de;
	struct stat st;
	// if equal name print path
	if(!strcmp(fmtname(path), targetname)){
		printf("%s\n", path);
	}
	// open file with read only mode
	// should open file before read
	fd = open(path, O_RDONLY);

	if (fd < 0) {
		return;
	}

	// get file infor
	if(fstat(fd, &st) < 0){
		close(fd);
		return;
	}
	
	// in case of file
	// not open normal file, just open dir file
	if(st.type != T_DIR){
		close(fd);
		return;
	}

	char buf[512];
	strcpy(buf, path);
	// copy path to buf

	// p is point to the end of buf
	char * p = buf + strlen(buf);

	// add '/' to the end of buf
	*p++ = '/';

	// logic just like ls.c
	while(read(fd, &de, sizeof(de)) == sizeof(de)){
		if (de.inum == 0) // not have file in this dir
			continue;
		memmove(p, de.name, DIRSIZ); // copy name to buf
		p[DIRSIZ] = 0; // add '\0' to the end of buf
		
		if (!strcmp(de.name, ".") || !strcmp(de.name, "..")) // ignore . and ..
			continue;

		find(buf, targetname); // recursive call find
	}
	close(fd);
}

int main(int argc, char *argv[]) {
	if(argc < 3){
		exit(1);
	}

	find(argv[1], argv[2]);

	exit(0);
}
