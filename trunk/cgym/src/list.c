#include <stdio.h>
#include "cgym.h"

int cgym_list(cgym_server_t *s, char *path) {
	printf("requesting list of directory %s from %s port %d\n",
		path, s->addr, s->port);
	
	return 0;
}
