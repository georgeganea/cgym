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
 	
 	if ((i = cgym_sock_connect(sock)) != 0) {
 		cgym_sock_info(sock);
 		printf("Error[%d]: Could not connect to ", i);
 		cgym_server_info_print( cgym_sock_get_server(sock) );
 		printf("\n");
 		return 2;
 	}
 	
 	if ((i = cgym_send_size_req(sock, remote)) != 0) {
 		cgym_sock_info(sock);
 		printf("Error[%d]: Could not request file information.\n", i);
 		return 3;
 	}
 	
 	if ((i = cgym_recv_size_reply(sock, &e)) != 0) {
 		cgym_sock_info(sock);
 		printf("Error[%d]: Could not get file information.\n", i);
 		return 4;
 	}
 	
 	cgym_entry_info(e);
 	printf("\n");
 	
 	cgym_sock_info(sock);
 	printf("\n"
 			"Avem informatiile. Cerem fisierul...\n");
 	
 	/*	
 	if ((segm = malloc(sizeof(*segm) * segments)) == NULL) {
 		perror("malloc");
 		return 8;
 	}
 	
 	for (i = 0; i < segments; i++) {
 	}
 	*/
 	
 	if ((i = cgym_send_get_req(sock,
 					cgym_segment_init(sock, e, 0, 1) )) != 0) {
 		cgym_sock_info(sock);
 		printf("Error[%d]: Could not request file content.\n", i);
 		return 3;
 	}
 	
 	if ((i = cgym_recv_get_reply(sock, NULL)) != 0) {
 		cgym_sock_info(sock);
 		printf("Error[%d]: Could not get file content.\n", i);
 		return 4;
 	}
 	
	return 0;
}
