#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

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
		if (lstat(d->d_name,&buf)!=0){
			fprintf(stderr,"lstat error");
			continue;
		}
		if (S_ISDIR(buf.st_mode)){
			printf("d - - %s%s\n",dirname,d->d_name);
		}
		else if (S_ISREG(buf.st_mode)){
			printf("- %ld %s%s\n",compute_md5(d->d_name),dirname,d->d_name);
		}
	}
}
