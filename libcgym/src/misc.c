#include <stdlib.h>
#include <string.h>

char *cgym_get_file_name(char *file) {
	char *ptr, *rc = NULL;
	
	if ((ptr = strrchr(file, '/')) == NULL) {
		ptr = file;
	} else {
		ptr++;
	}
	
	rc = malloc(strlen(ptr) + 1);
		
	if (rc != NULL) {
		strcpy(rc, ptr);
	}
	
	return rc;
}
