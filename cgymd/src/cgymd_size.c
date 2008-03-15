#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include "libcgym.h"
#include "cgymd.h"

/*
 * The function returns the size of the file given as argument
 */

cgym_entry_t* size(char* path, char* filename){
	struct stat buf;
	char* pathname;
	if ((pathname = malloc(strlen(path)+strlen(filename)+1))==NULL){
			return NULL;
	}
	strcpy(pathname, path);
	strcat(pathname, filename);
	if (stat(pathname,&buf)<0){
		fprintf(stderr,"stat error\n");
		return NULL;
	}
	if (S_ISREG(buf.st_mode)){
		cgym_entry_t* entry;
		entry = cgym_entry_init(filename, compute_md5(pathname), CGYM_ENTRY_FILE, buf.st_size);
		return entry;
	}
	return NULL;
}
