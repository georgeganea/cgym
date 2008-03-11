#include <stdio.h>
#include "cgym.h"

int cgym_list(cgym_server_t *s, char *path) {
	printf("requesting list of directory %s from\n", path);
	
	cgym_server_info_print(s);
	
	printf("\n");
	
	return 0;
}
