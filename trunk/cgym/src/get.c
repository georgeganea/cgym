#include <stdio.h>
#include "cgym.h"

int cgym_get(char *remote, char *local, int segments, cgym_server_t **servers) {
	// afisam niste informatii
	printf("getting '%s' in %d segments, saving to '%s'\n",
		remote, segments, local);
	
	printf("servers:\n");
 	while (*servers != NULL) {
 		putchar('\t');
 		cgym_server_info_print(*(servers++));
 		putchar('\n');
 	}
 	
 	
	
	return 0;
}
