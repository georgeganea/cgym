#include <stdio.h>
#include "cgym.h"

int cgym_get(char *remote, char *local, int segments, cgym_server_t **servers) {
	cgym_sock_t *sock;
	cgym_server_t **serv;
	
	// afisam niste informatii
	printf("getting '%s' in %d segments, saving to '%s'\n",
		remote, segments, local);
	
	printf("servers:\n");
	serv = servers;
 	while (*serv != NULL) {
 		putchar('\t');
 		cgym_server_info_print(*(serv++));
 		putchar('\n');
 	}
 	
 	if ((sock = cgym_sock_create( *servers )) == NULL) {
 		printf("Error: could not create socket.\n");
 		return 1;
 	}
 	
 	if ((cgym_sock_connect(sock))) {
 		cgym_sock_print_info(sock);
 		printf("Error: Could not connect to ");
 		cgym_server_info_print( cgym_sock_get_server(sock) );
 		printf("\n");
 		return 2;
 	}
 	
 	printf("GATA!\n");
	
	return 0;
}
