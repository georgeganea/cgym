#ifndef CGYMD_H_
#define CGYMD_H_
#include "libcgym.h"

/*
 * Structure and function for obtaining the contents of a directory
 */
typedef struct file_info FILE_INFO;

struct file_info {
	cgym_entry_t *entry_file;
	FILE_INFO *next;
};

FILE_INFO* list(char* dirname);

/*
 * Fuction for reading the contents of a file
 */

char* get(char* start, char* stop, char* filename);

#endif /*CGYMD_H_*/
