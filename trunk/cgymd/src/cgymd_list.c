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

FILE_INFO* list(char* dirpath){
	DIR *dp = NULL;
	struct dirent *d;
	struct stat buf;
	FILE_INFO *files_head = NULL;
	char* dirname;
	char* last = strrchr(dirpath,'/');
	
	if (strlen(last) > 2){
		dirname = malloc(strlen(dirpath)+2);
		strcpy(dirname,dirpath);
		strcat(dirname,"/");
	}
	else{
		dirname = malloc(strlen(dirpath)+1);
		strcpy(dirname,dirpath);		
	}
	printf("%s",dirname);
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
			 char* dir;
			 dir = malloc(strlen(dirname)+strlen(d->d_name)+1);
			 strcpy(dir,dirname);
			 strcat(dir,d->d_name);
			 //sprintf(dir,"%s%s",dirname,d->d_name);
			 cgym_entry_t* entry;
			 entry = cgym_entry_init(dir,"-",CGYM_ENTRY_DIRECTORY,0);
			 files_head = add_entry(entry,files_head);
			 free(dir);
		 }
		 else if (S_ISREG(buf.st_mode)){
			 char* file;
			 file = malloc(strlen(dirname)+strlen(d->d_name)+1);
			 strcpy(file,dirname);
			 strcat(file,d->d_name);
			 cgym_entry_t* entry;
			 entry = cgym_entry_init(file,compute_md5(d->d_name),CGYM_ENTRY_FILE,buf.st_size);
			 files_head = add_entry(entry, files_head);
			 free(file);
		 }
	 }
	 closedir(dp);
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
