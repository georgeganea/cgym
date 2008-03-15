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

cgym_entry_t* size(char* filename){
	struct stat buf;
	if (stat(filename,&buf)<0){
		fprintf(stderr,"Stat error\n");
		return NULL;
	}
	if (S_ISREG(buf.st_mode)){
		cgym_entry_t* entry;
		entry = cgym_entry_init(filename, compute_md5(filename), CGYM_ENTRY_FILE, buf.st_size);
		return entry;
	}
	return NULL;
}
