#ifndef CGYMD_H_
#define CGYMD_H_
#include "libcgym.h"

typedef struct file_info FILE_INFO;

struct file_info {
	cgym_entry_t *entry_file;
	FILE_INFO *next;
};

#endif /*CGYMD_H_*/
