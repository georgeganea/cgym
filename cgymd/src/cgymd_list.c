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
 * The function returns a list of files and directories in dirname
 * On success, the first element of the list is the on that was requested
 * information for. On error, the function returns null;
 */

FILE_INFO* add_entry(cgym_entry_t* entry, FILE_INFO* head);

FILE_INFO* list(char* dirname){
	DIR *dp = NULL;
	struct dirent *d;
	struct stat buf;
	FILE_INFO *files_head = NULL;
	
	printf("Directorul:%s\n",dirname);
	if ((dp = opendir(dirname))==NULL){
		perror("opendir");
		return NULL;
	}
	 if (chdir(dirname) !=0){
	    perror("chdir");
	    return NULL;
	 }
	 cgym_entry_t* first_entry;
	 first_entry = cgym_entry_init(dirname,NULL,CGYM_ENTRY_NONE,0);
	 files_head = add_entry(first_entry, files_head);
	 
	 while((d = readdir(dp))!=NULL){
		 if (d->d_ino == 0)
			 continue;
		 if ((strcmp(d->d_name,".")==0) ||(strcmp(d->d_name,"..")==0))
			 continue;
		 if (stat(d->d_name,&buf)<0){
			 fprintf(stderr,"lstat error");
			 continue;
		 }
		 if (S_ISDIR(buf.st_mode)){
			 cgym_entry_t* entry;
			 entry = cgym_entry_init(d->d_name,"-",CGYM_ENTRY_DIRECTORY,buf.st_size);
			 files_head = add_entry(entry,files_head);
		 }
		 else if (S_ISREG(buf.st_mode)){
			 cgym_entry_t* entry;
			 entry = cgym_entry_init(d->d_name,compute_md5(d->d_name),CGYM_ENTRY_FILE,buf.st_size);
			 files_head = add_entry(entry, files_head);
		 }
	 }
	 return files_head;
}

FILE_INFO* add_entry(cgym_entry_t* entry, FILE_INFO* head){
	FILE_INFO *files_curr;
	 if ((files_curr =(FILE_INFO *) malloc(sizeof(FILE_INFO))) == NULL) {
	 		fprintf(stderr,"Eroare la malloc\n");
	 		return NULL;
	 }
	 files_curr->entry_file = entry;
	 files_curr->next = head;
	 head = files_curr;
	 return head;
}
