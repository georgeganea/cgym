#include <stdlib.h>
#include <string.h>
#include "libcgym.h"
#include "libcgym_priv.h"

/*
 * trimite cererea pentru informatia despre fisier
 *
 * Returneaza
 * 	0 la succes
 * 	>1 la eroare
 */
int cgym_send_size_req(cgym_sock_t *sock, char *file) {
	int rc = 0;
	char *buf;
	
	puts(__func__);
	cgym_sock_info(sock);
	printf("\nfile = '%s'\n", file);
	
	if (sock != NULL) {
		if ((buf = malloc(strlen(CGYM_SIZE_MSG) + strlen(file) + 1)) != NULL) {
			sprintf(buf, CGYM_SIZE_MSG, file);
			// TODO: fa-l cu select()
			
			do {
				rc = cgym_send(sock, buf, strlen(buf));
			} while (rc == 1); // inca nu a terminat
			
			if (rc == 0)
				sock->state = CGYM_SOCK_RECV_SIZE_REPLY;
			free(buf);
		} else {
			rc = 3;
		}
	} else {
		rc = 2;
	}
	
	return rc;
}

/*
 * citeste de pe socket informatia despre fisier
 * trimisa de server si o salveaza in e
 *
 * returneaza:
 *	0 la succes
 *	1 la incomplet
 *	2 la eroare
 */
int cgym_recv_size_reply(cgym_sock_t *sock, cgym_entry_t **e) {
	int rc = 0;
	char *ptr;
	cgym_entry_t *entry = NULL;
	
	if (sock != NULL) {
		if (e != NULL) {
			if (sock->state == CGYM_SOCK_RECV_SIZE_REPLY) {
				rc = cgym_recv(sock, strlen(CGYM_OK_MSG));
				
				if (rc == 0) {
					// am primit tot raspunsul (OK ?)
					if (strncmp(sock->buf, CGYM_OK_MSG, strlen(CGYM_OK_MSG))) {
						// serverul a raspuns (probabil) cu ERR\r\n
						rc = 2;
#ifdef DEBUG_CGYM
						printf("NU E OK: %4s...\n", sock->buf);
#endif
						sock->state = CGYM_SOCK_CONNECTED;
					} else {
#ifdef DEBUG_CGYM
						printf("E OK: pos_recv = %ld\n", sock->pos_recv);
#endif
						sock->state = CGYM_SOCK_RECV_SIZE_DATA;
					}
				} else if (rc > 1) {
					sock->state = CGYM_SOCK_CONNECTED;
				} // else -- incomplet
			}

			if (sock->state == CGYM_SOCK_RECV_SIZE_DATA) {
#ifdef DEBUG_CGYM
				char tmp[20];
				printf("pos_recv = %ld\n", sock->pos_recv);
#endif
				rc = cgym_recv(sock, MAX_LINE_LEN + 1); // ca sa fim siguri ca primim tot
#ifdef DEBUG_CGYM
				printf("recv(sock, %d) = %d\n", MAX_LINE_LEN + 1, rc);
				printf("pos_recv = %ld\n", sock->pos_recv);
				sprintf(tmp, "buf[%d-%ld]: `%%.%lds'\n",
						0, sock->pos_recv,
						sock->pos_recv);
				printf(tmp, sock->buf);
#endif
				
				if (rc == 1) {
					// am primit ceva?
					if (sock->pos_recv > 0) {
						if (
							(ptr = memchr(sock->buf, '\r', sock->pos_recv)) != NULL
							&&
							(memchr(sock->buf, '\n', sock->pos_recv)) == ptr+1
							)
						{
							/*
							 * am gasit un sfarsit de linie
							 * 
							 * linia care ne intereseaza incepe de la sock->buf
							 * si se termina la ptr+2 (exclusiv)
							 */
							if ((entry = cgym_entry_init_raw(sock->buf))
																	!= NULL) {
								*e = entry;
								
								// am terminat
								sock->state = CGYM_SOCK_CONNECTED;
								cgym_sock_clear(sock);
								rc = 0;
							} else {
								rc = 5;
							}
						} else {
							// mai trebuie sa citim
#ifdef DEBUG
							printf("incomplet. mai trebuie citit...\n");
#endif
						}
					} else {
#ifdef DEBUG_CGYM
						printf("nu avem ce citi pe moment!\n");
#endif
					}
				} else if (rc == 0) {
					// TODO: lungimea e fix MAX_LINE_LEN+1
#ifdef DEBUG_CGYM
					printf("Nu se poate!\n");
#endif
					rc = 4;
				}
			}
		} else {
			rc = 3;
		}
	} else {
		rc = 2;
	}
	
	if (rc > 1) { // o eroare -- eliberam buffer-ul
#ifdef DEBUG_CGYM
		printf("Eliberam buffer-ul...\n");
#endif
		cgym_sock_clear(sock);
	}
	
	return rc;
}

/*
 * trimite cererea pentru continutul fisierului
 *
 * Returneaza
 * 	0 la succes
 * 	>1 la eroare
 */
int cgym_send_get_req(cgym_segment_t *s) {
	int rc = 0;
	char *buf, *filename;
	
	if (s != NULL) {
		if (s->sock != NULL) {
			filename = cgym_entry_file( cgym_segment_entry(s) );
			
			if (filename != NULL) {
				if ((buf = malloc(strlen(CGYM_GET_MSG)
								+ strlen(filename) + 21)) != NULL) {
#ifdef DEBUG_CGYM
					printf("creating GET request: %ld, %ld, %s\n",
							s->start, s->stop, filename);
#endif
					sprintf(buf, CGYM_GET_MSG, s->start, s->stop, filename);

					// TODO: fa-l cu select()
					do {
						rc = cgym_send(s->sock, buf, strlen(buf));
					} while (rc == 1); // inca nu a terminat
					
					if (rc == 0) // am putut trimite -- schimbam starea
						s->sock->state = CGYM_SOCK_RECV_GET_REPLY;
					free(buf);
				} else {
					rc = 5;
				}
			} else {
				rc = 4;
			}
		} else {
			rc = 3;
		}
	} else {
		rc = 2;
	}
	
	return rc;
}

/*
 * citeste de pe socket raspunsul serverului si
 * continutul fisierului si il salveaza in s
 *
 * returneaza:
 *	0 la succes
 *	1 la incomplet
 *	2 la eroare
 */
int cgym_recv_get_reply(cgym_segment_t *s) {
	int rc = 0;
	cgym_sock_t *sock;
	cgym_entry_t *e;
	
	if (s != NULL) {
		sock = s->sock;
		e = s->entry;

		if (sock != NULL) {
			if (e != NULL) {
				if (sock->state == CGYM_SOCK_RECV_GET_REPLY) {
#ifdef DEBUG_CGYM
					char tmp[20];
					printf("pos_recv = %ld\n", sock->pos_recv);
#endif
					rc = cgym_recv(sock, strlen(CGYM_OK_MSG));
#ifdef DEBUG_CGYM
					printf("recv(sock, %d) = %d\n", strlen(CGYM_OK_MSG), rc);
					printf("pos_recv = %ld\n", sock->pos_recv);
					sprintf(tmp, "buf[%d-%d]: `%%.%ds'\n",
							0, strlen(CGYM_OK_MSG),
							strlen(CGYM_OK_MSG));
					printf(tmp, sock->buf);
#endif
					
					if (rc == 0) {
						// am primit tot raspunsul (OK ?)
						if (strncmp(sock->buf, CGYM_OK_MSG, strlen(CGYM_OK_MSG))) {
							// serverul a raspuns (probabil) cu ERR\r\n
							rc = 2;
#ifdef DEBUG_CGYM
							printf("NU E OK: %4s...\n", sock->buf);
#endif
							sock->state = CGYM_SOCK_CONNECTED;
						} else {
#ifdef DEBUG_CGYM
							printf("E OK: pos_recv = %ld\n", sock->pos_recv);
#endif
							s->status = CGYM_SEGMENT_STARTED;
							sock->state = CGYM_SOCK_RECV_GET_DATA;
						}
					} else if (rc > 1) {
						// TODO: poate e deja ERR ?
						sock->state = CGYM_SOCK_CONNECTED;
						s->status = CGYM_SEGMENT_IDLE;
					} // else -- incomplet
				}
	
				if (sock->state == CGYM_SOCK_RECV_GET_DATA) {
#ifdef DEBUG_CGYM
					char tmp[50];
					printf("pos_recv = %ld\n", sock->pos_recv);
#endif
					rc = cgym_recv(sock, s->stop - s->start + 2); // plus \r\n la sfarsit
#ifdef DEBUG_CGYM
					printf("recv(sock, %ld) = %d\n", s->stop - s->start + 2, rc);
					printf("pos_recv = %ld\n", sock->pos_recv);
#endif
					
					if (rc == 0) {
#ifdef DEBUG_CGYM
						sprintf(tmp, "buf[%ld-%ld]: `%%.%lds'\n",
								s->start, s->stop + 2,
								s->stop - s->start + 2);
						printf(tmp, sock->buf);
#endif
						if (sock->buf[s->stop - s->start] == '\r'
							&& sock->buf[s->stop - s->start + 1] == '\n') {
#ifdef DEBUG_CGYM
							printf("am primit tot fisierul (si \\r\\n la sfarsit)\n"
									"buf: %.4s...\n", sock->buf);
#endif
							
							s->buf = sock->buf;
							s->sock->state = CGYM_SOCK_CONNECTED;
							s->status = CGYM_SEGMENT_DONE;
						} else {
#ifdef DEBUG_CGYM
							printf("Eroare: segmentul nu se termina cu \\r\\n\n");
#endif
							rc = 5;
						}
					} else if (rc > 1) {
						s->status = CGYM_SEGMENT_IDLE;
						rc = 4;
					} else {
#ifdef DEBUG_CGYM
						sprintf(tmp, "(i) buf[%ld-%ld]: `%%.%lds'\n",
								s->start, s->stop + 2,
								sock->pos_recv);
						printf(tmp, sock->buf);
#endif
					}
				}
			} else {
				rc = 3;
			}
		} else {
			rc = 2;
		}
		
		if (rc > 1) { // o eroare -- eliberam buffer-ul
#ifdef DEBUG_CGYM
			printf("Eliberam buffer-ul...\n");
#endif
			cgym_sock_clear(sock);
		}
	}
	
	return rc;
}

cgym_sock_t *cgym_sock_setup(cgym_server_t **curr, cgym_server_t **head) {
	cgym_sock_t *sock;
	int ret, done = 0;

	while (head != NULL && !done) {
		if (curr == NULL)
			curr = head;
	 	
		if ((sock = cgym_sock_create(*curr)) == NULL) {
	 		printf("Error: could not create socket.\n");
	 		
	 		cgym_server_remove(curr);
	 		continue; // nu am putut crea socket (host inexistent, sistem ?)
	 	}

	 	if ((ret = cgym_sock_setblocking(sock, 0)) != 0) {
	 		printf("Error[%d]: Could not set socket to nonblocking mode\n", ret);
	 		cgym_sock_info(sock);
	 		
	 		cgym_sock_free(sock);
	 		return NULL; // eroare mai grava, nu am putut pune nonblocking
	 	}
		
	 	if ((ret = cgym_sock_connect(sock)) > 1) {
	 		cgym_sock_info(sock);
	 		printf("Error[%d]: Could not connect to ", ret);
	 		cgym_server_info_print( cgym_sock_get_server(sock) );
	 		
	 		cgym_sock_free(sock);
	 		cgym_server_remove(curr);
	 		continue; // trecem la serverul urmator
	 	}
	 	
	 	done = 1; // gata, iesim!
	}
 	
 	return sock;
}
