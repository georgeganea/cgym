#include <stdio.h>
#include "cgym.h"

int cgym_get(char *remote, char *local, int segments, cgym_server_t **servers) {
	printf("getting '%s' in %d segments, saving to '%s'\n",
		remote, segments, local);
	
	printf("servers:\n");
 	while (*servers != NULL) {
 		printf("\t%s port %d\n", (*servers)->addr, (*servers)->port);
 		servers++;
 	}
	
	return 0;
}
