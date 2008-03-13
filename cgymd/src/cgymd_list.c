#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include "libcgym.h"

void list(char* dirname){
	DIR *dp = NULL;
	struct dirent *d;
	struct stat buf;
	
	printf("Directorul:%s\n",dirname);
	if ((dp = opendir(dirname))==NULL){
		perror("opendir");
		return;
	}
	 if (chdir(dirname) !=0){
	    perror("chdir");
	    return;
	 }
	while(d = readdir(dp)){
		if (d->d_ino == 0)
			continue;
		if ((strcmp(d->d_name,".")==0) ||(strcmp(d->d_name,"..")==0))
				continue;
		if (lstat(d->d_name,&buf)!=0){
			fprintf(stderr,"lstat error");
			continue;
		}
		if (S_ISDIR(buf.st_mode)){
			cgym_entry_t* entry;
			entry =	cgym_entry_init(d->d_name,"-",CGYM_ENTRY_DIRECTORY,buf.st_size);
			printf("%s\n",cgym_entry_file(entry));
		}
		else if (S_ISREG(buf.st_mode)){
			cgym_entry_t* entry;
			entry = cgym_entry_init(d->d_name,compute_md5(d->d_name),CGYM_ENTRY_FILE,buf.st_size);
			printf("%s\n",cgym_entry_file(entry));
		}
	}
}
