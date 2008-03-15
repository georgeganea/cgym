#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include "libcgym.h"
#include "cgymd.h"

char* get(char* start, char* stop, char* path, char* filename){
	unsigned long start_position;
	unsigned long end_position;
	char* ep;
	
	char* pathname;
	if ((pathname = malloc(strlen(path)+strlen(filename)+1))==NULL){
		return NULL;
	}
	strcpy(pathname, path);
	strcat(pathname, filename);
	
	start_position = strtoul(start, &ep, 10);
	
	if (*start == '\0' || *ep != '\0' || *start == '-'){
		fprintf(stderr,"Not a number\n");
		return NULL; //got not a number
	}
	if (errno == ERANGE && start_position == ULONG_MAX){
		fprintf(stderr,"Out of range\n");
		return NULL; // got out of range
	}
	
	end_position = strtoul(stop, &ep, 10);
	
	if (*stop == '\0' || *ep != '\0' || *stop == '-'){
		fprintf(stderr,"Not a number\n");
		return NULL; //got not a number
	}
	if (errno == ERANGE && end_position == ULONG_MAX){
		fprintf(stderr,"Out of range\n");
		return NULL; // got out of range
	}
	
	if (start_position >= end_position )
		return NULL;
	FILE* f;
	if ((f= fopen(pathname, "r"))==NULL){
		fprintf(stderr,"Eroare la fopen\n");
		return NULL;
	}
	if (fseek(f,start_position,SEEK_SET)<0){
		fprintf(stderr,"Eroare la fseek\n");
		return NULL;		
	}
	char* buff = malloc(end_position-start_position+1);
	if (fread(buff,sizeof(char),end_position-start_position,f)<end_position-start_position){
		fprintf(stderr,"Eroare la fread");
		return NULL;
	}
	*(buff+(end_position-start_position)) = '\0';
	fclose(f);
	return buff;
}

