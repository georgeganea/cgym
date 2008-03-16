#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include "libcgym.h"

int cgym_get(char *remote, char *local, int nr_segm, cgym_server_t **servers) {
	cgym_sock_t *sock;
	cgym_server_t **serv;
	cgym_segment_t **segm;
	cgym_entry_t *e, *e1;
	
    fd_set readfds, writefds, exceptfds;
	
	int i, ret, rc, sockfd, max_sockfd;
	unsigned long segm_size, segm_start, segm_stop;
	
	enum cgym_sock_state state; 
	
	// afisam niste informatii
	printf("getting '%s' in %d segments, saving to '%s'\n",
		remote, nr_segm, local);
	
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
 	
 	if ( cgym_entry_type(e) != CGYM_ENTRY_FILE ) {
 		printf("Error: entry is not a file.\n");
 		return 5;
 	}
 	
 	printf("\n"
 			"Avem informatiile. Cerem fisierul...\n");
 	
 	printf("Alocam memorie pentru segmente...\n");
 	
 	if ((segm = malloc(sizeof(*segm) * (nr_segm + 1))) == NULL) {
 		perror("malloc");
 		return 8;
 	}
 	
 	serv = servers+1;
 	segm_size = cgym_entry_size(e) / nr_segm;
 	
 	printf("len=%ld, nr_segm=%d, segm_size=%ld\n",
				cgym_entry_size(e), nr_segm, segm_size);
 	 	
	for (i = 0; i < nr_segm; i++) {
 		if (i == 0) {
	 	 	if ((rc = cgym_sock_setblocking(sock, 0)) != 0) {
	 	 		printf("Error[%d]: Could set socket to nonblocking mode\n", rc);
	 	 		cgym_sock_info(sock);
	 	 		printf("\n");
	 	 		return 2;
	 	 	}
 	 	} else {
	 		if (*serv == NULL) {
	 			serv = servers;
	 		}
	 		 		
	 	 	if ((sock = cgym_sock_create( *(serv++) )) == NULL) {
	 	 		printf("Error: could not create socket.\n");
	 	 		return 1;
	 	 	}
	 	 	
	 	 	if ((rc = cgym_sock_setblocking(sock, 0)) != 0) {
	 	 		printf("Error[%d]: Could set socket to nonblocking mode\n", rc);
	 	 		cgym_sock_info(sock);
	 	 		printf("\n");
	 	 		return 2;
	 	 	}
	 	 	
	 	 	if ((rc = cgym_sock_connect(sock)) > 1) {
	 	 		cgym_sock_info(sock);
	 	 		printf("Error[%d]: Could not connect to ", rc);
	 	 		cgym_server_info_print( cgym_sock_get_server(sock) );
	 	 		printf("\n");
	 	 		return 2;
	 	 	}
 		}
 	 	
 	 	segm_start = i * segm_size;
 	 	
 	 	if (i < nr_segm - 1) { // ultimul segment ia si restul
 	 		segm_stop = (i+1) * segm_size;
 	 	} else {
 	 		segm_stop = cgym_entry_size(e);
 	 	}
 	 	
 	 	printf("segmentul %d va cere (%ld, %ld)\n", i, segm_start, segm_stop);

 	 	segm[i] = cgym_segment_init(sock, e, segm_start, segm_stop);
 	 	
 	 	if (i == 0) {
 	 		if (cgym_send_get_req(segm[i])) {
 	 			printf("Error: could not send GET request.\n");
 	 			// TODO: create new connection in its place
 	 			exit(1);
 	 		}
 	 	}
 	}
 	segm[i] = NULL;
 	
 	while (!cgym_segment_done(segm)) {
 		// initializam seturile pentru select()
 		FD_ZERO(&readfds);
 		FD_ZERO(&writefds);
 		FD_ZERO(&exceptfds);
 		
 		printf("==== INIT ====\n");
 		
 		max_sockfd = 0;
 		for (i = 0; i < nr_segm; i++) {
 			sock = cgym_segment_sock(segm[i]);
 			sockfd = cgym_sock_get_sockfd(sock);
 			state = cgym_sock_get_state(sock);
 			
 			cgym_sock_info(sock); printf("\n");
 			
 			if (max_sockfd < sockfd) max_sockfd = sockfd;
 			
 			switch (state) {
 				/* inca asteptam sa ne conectam,
 				 * deci doar putem afla ca s-a schimbat starea
 				 */
 				case CGYM_SOCK_CONNECTING:
 					FD_SET(sockfd, &writefds);
 					break;

 				/* cum scrierile se fac blocking,
 				 * asteptam doar cititul
 				 */
 				case CGYM_SOCK_RECV_HANDSHAKE:
 				case CGYM_SOCK_RECV_SIZE_REPLY:
 				case CGYM_SOCK_RECV_SIZE_DATA:
 				case CGYM_SOCK_RECV_GET_REPLY:
 				case CGYM_SOCK_RECV_GET_DATA:
 					FD_SET(sockfd, &readfds);
 					break;
 			
 				// eroare? trebuie repornit segmentul
 				case CGYM_SOCK_ERR:
 					printf("ERROR!\n");
 					exit(1);
 					break;
 				
 				default:
 					/*
 					 * adica
 					 * CGYM_SOCK_CONNECTED,
 					 * CGYM_SOCK_RECV_LIST,
 					 * CGYM_SOCK_NONE,
 					 * CGYM_SOCK_IDLE
 					 * 
 					 * care nu ar trebui sa apara la inceputul ciclului
 					 */
 					printf("Unhandled state: ");
 					cgym_sock_info(sock);
 					printf("\n");
 					exit(1);
 			}
 		}
 		
 		// asteptam o modificare cu select()
	    if ((rc = select(max_sockfd+1,
	    					&readfds, &writefds, &exceptfds, NULL)) < 0) {
	    	perror("select");
	    	exit(1);
	    }
	    
	    printf("### %d eventuri\n", rc);
 
 		for (i = 0; i < nr_segm; i++) {
 			sock = cgym_segment_sock(segm[i]);
 			
 			sockfd = cgym_sock_get_sockfd(sock);
 			state = cgym_sock_get_state(sock);
 			
 			printf("SOCK (Before)[%d]: ", i);
 			cgym_sock_info(sock);
 			printf("\n");
 			
 			if (FD_ISSET(sockfd, &exceptfds)) {
 				printf("%d: OOB data? ", i);
 				cgym_sock_info(sock);
 				printf("\n");
 				rc--;
 			}

 			if (FD_ISSET(sockfd, &readfds)) {
 				printf("%d: putem citi ", i);
 				cgym_sock_info(sock);
 				printf("\n");
 				
 				switch (state) {
	 				case CGYM_SOCK_RECV_HANDSHAKE:
						/*
						 * inca nu am primit handshake.
						 * dar am primit acuma ceva, deci mai putem insista...
						 */
	 				 	ret = cgym_sock_connect(sock);
	 				 	
	 				 	if (ret == 0) { // conectat
	 				 		// cere informatii
	 				 		if ((ret = cgym_send_size_req(sock,
	 				 							cgym_entry_file(e))) != 0) {
		 				 		cgym_sock_info(sock);
		 				 		printf("Error[%d]: Could not send SIZE request ", ret);
		 				 		cgym_server_info_print( cgym_sock_get_server(sock) );
		 				 		printf("\n");
		 				 		return 2;
	 				 		}
	 				 	} else if (ret > 1) { // eroare
	 				 		cgym_sock_info(sock);
	 				 		printf("Error[%d]: Could not connect to ", ret);
	 				 		cgym_server_info_print( cgym_sock_get_server(sock) );
	 				 		printf("\n");
	 				 		return 2;
	 				 	}
	 				 	
	 					break;
	
	 				case CGYM_SOCK_RECV_SIZE_REPLY:
	 				case CGYM_SOCK_RECV_SIZE_DATA:
	 					ret = cgym_recv_size_reply(sock, &e1);
	 					
	 					if (ret == 0) { // gata
	 						if (cgym_entry_type(e1) == CGYM_ENTRY_FILE
	 							&& !strcmp(cgym_entry_file(e), cgym_entry_file(e1))
	 							&& cgym_entry_size(e) == cgym_entry_size(e1)
	 							&& !strcmp(cgym_entry_md5(e), cgym_entry_md5(e1))
	 							) {
	 							// e acelasi fisier
	 				 	 		if (cgym_send_get_req(segm[i])) {
	 				 	 			printf("Error: could not send GET request.\n");
	 				 	 			// TODO: create new connection in its place
	 				 	 			exit(1);
	 				 	 		}
	 						} else {
	 							// nu e acelasi fisier -- trebuie sters serverul
	 							// TODO: sterge serverul
	 						}
	 					} else if (ret > 1) { // eroare
	 						cgym_sock_info(sock);
	 				 		printf("Error[%d]: Could not receive SIZE reply ", ret);
	 				 		cgym_server_info_print( cgym_sock_get_server(sock) );
	 				 		printf("\n");
	 				 		return 2;
	 				 	}
	 					break;
	 				case CGYM_SOCK_RECV_GET_REPLY:
	 				case CGYM_SOCK_RECV_GET_DATA:
	 					ret = cgym_recv_get_reply(segm[i]);
	 					
	 					if (ret == 0) { // gata
	 						printf("GATA?!\n");
	 						exit(1);
	 					} else if (ret > 1) {
	 						cgym_sock_info(sock);
	 				 		printf("Error[%d]: Could not receive GET reply ", ret);
	 				 		cgym_server_info_print( cgym_sock_get_server(sock) );
	 				 		printf("\n");
	 				 		return 2;
	 					}
	 					break;
	 				
	 				default:
	 					/*
	 					 * adica
	 					 * CGYM_SOCK_CONNECTING,
	 					 * CGYM_SOCK_CONNECTED,
	 					 * CGYM_SOCK_RECV_LIST,
	 					 * CGYM_SOCK_NONE,
	 					 * CGYM_SOCK_IDLE,
	 					 * CGYM_SOCK_ERR
	 					 * 
	 					 * care nu ar trebui sa apara la inceputul ciclului
	 					 */
	 					printf("(read) Unhandled state: ");
	 					cgym_sock_info(sock);
	 					printf("\n");
	 					exit(1);
 				}

 				rc--;
 			}

 			if (FD_ISSET(sockfd, &writefds)) {
 				printf("%d: putem scrie ", i);
 				cgym_sock_info(sock);
 				printf("\n");
 				
 				switch (state) {
 					case CGYM_SOCK_CONNECTING:
						/*
						 * inca nu ne-am conectat
						 * dar am primit acuma ceva, deci mai putem insista...
						 */
	 				 	ret = cgym_sock_connect(sock);
	 				 	
	 				 	if (ret == 0) { // conectat
	 				 		// cere informatii
	 				 		if ((ret = cgym_send_size_req(sock,
	 				 							cgym_entry_file(e))) != 0) {
		 				 		cgym_sock_info(sock);
		 				 		printf("Error[%d]: Could not send SIZE request ", ret);
		 				 		cgym_server_info_print( cgym_sock_get_server(sock) );
		 				 		printf("\n");
		 				 		return 2;
	 				 		}
	 				 	} else if (ret > 1) { // eroare
	 				 		cgym_sock_info(sock);
	 				 		printf("Error[%d]: Could not connect to ", ret);
	 				 		cgym_server_info_print( cgym_sock_get_server(sock) );
	 				 		printf("\n");
	 				 		return 2;
	 				 	}

	 				 	break;
 					
					default:
	 					printf("(write) Unhandled state: ");
	 					cgym_sock_info(sock);
	 					printf("\n");
	 					exit(1);
 				}
 				
 				rc--;
 			}
 			
 			printf("SOCK (After)[%d]: ", i);
 			cgym_sock_info(sock);
 			printf("\n\n");

 			if (rc == 0) // am procesat toate evenimentele
 				break;
 		}
 	}
 	
 	if (cgym_segment_done(segm) == -1) {
 		printf("Eroare: Unexpected segment status.\n");
 	}
 	
 	/*
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
 	*/
 	
 	// cgym_segment_assemble(fp, segm);
 	
 	for (i = 0; i < nr_segm; i++) {
 		printf("sock_info: ");
 		cgym_sock_info( cgym_segment_sock(segm[i]));
 		printf("\n");
 		
 		cgym_sock_free( cgym_segment_sock(segm[i]) );
 		cgym_segment_free(segm[i]);
 	}
 	
 	cgym_entry_free(e);
 	
 	free(segm);
 	
	return 0;
}
