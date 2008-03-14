#include <stdio.h>
#include <stdlib.h>
#include "libcgym.h"

int cgym_get(char *remote, char *local, int segments, cgym_server_t **servers) {
	cgym_sock_t *sock;
	cgym_server_t **serv;
	cgym_segment_t **segm;
	cgym_entry_t *e;
	
	int i;
	
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
 	
 	if (cgym_send_size_req(sock, remote)) {
 		cgym_sock_print_info(sock);
 		printf("Error: Could not request file information.\n");
 		return 3;
 	}
 	
 	if (cgym_recv_size_reply(sock, &e)) {
 		cgym_sock_print_info(sock);
 		printf("Error: Could not get file information.\n");
 		return 4;
 	}
 	
 	printf("entry info: %s [%ld bytes, md5: %s]\n",
 			cgym_entry_file(e), cgym_entry_size(e), cgym_entry_md5(e));
 	
 	if ((segm = malloc(sizeof(*segm) * segments)) == NULL) {
 		perror("malloc");
 		return 8;
 	}
 	
 	for (i = 0; i < segments; i++) {
 	}
 	
 	printf("GATA!\n");
	
	return 0;
}
