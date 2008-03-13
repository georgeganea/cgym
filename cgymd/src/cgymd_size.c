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


char* size(char* filename){
	struct stat buf;
	printf("In size\n");
	char* file_size = malloc(2*sizeof(unsigned long)*8+1);
	printf("%d\n",sizeof(unsigned long));
	if (stat(filename,&buf)<0){
		fprintf(stderr,"Stat error\n");
		return NULL;
	}
	if (S_ISREG(buf.st_mode)){
		sprintf(file_size,"%lu",buf.st_size);
		return file_size;
	}
	return NULL;
}