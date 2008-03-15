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
	
	if (sock != NULL) {
		if ((buf = malloc(strlen(CGYM_SIZE_MSG) + strlen(file) + 1)) != NULL) {
			sprintf(buf, CGYM_SIZE_MSG, file);
			
			do {
				rc = cgym_send(sock, buf, strlen(buf));
			} while (rc == 1); // inca nu a terminat
			
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
			if (sock->state == CGYM_SOCK_CONNECTED) { // cerere noua
				sock->state = CGYM_SOCK_RECV_SIZE_REPLY;
			}
			
			if (sock->state == CGYM_SOCK_RECV_SIZE_REPLY) {
				rc = cgym_recv(sock, strlen(CGYM_OK_MSG));
				
				if (rc == 0) {
					// am primit tot raspunsul (OK ?)
					if (strncmp(sock->buf, CGYM_OK_MSG, strlen(CGYM_OK_MSG))) {
						// serverul a raspuns (probabil) cu ERR\r\n
						rc = 2;
						printf("NU E OK: %4s...\n", sock->buf);
						sock->state = CGYM_SOCK_CONNECTED;
					} else {
						printf("E OK: pos_recv = %ld\n", sock->pos_recv);
						sock->state = CGYM_SOCK_RECV_SIZE_DATA;
					}
				} else if (rc > 1) {
					sock->state = CGYM_SOCK_CONNECTED;
				} // else -- incomplet
			}

			if (sock->state == CGYM_SOCK_RECV_SIZE_DATA) {
				rc = cgym_recv(sock, MAX_LINE_LEN + 1); // ca sa fim siguri ca nu primim tot
				
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
							printf("incomplet. mai trebuie citit...\n");
						}
					} else {
						printf("nu avem ce citi pe moment!\n");
					}
				} else if (rc == 0) {
					printf("Nu se poate!\n");
					rc = 4;
				} else {
					sock->state = CGYM_SOCK_CONNECTED;
				}
			}
		} else {
			rc = 3;
		}
	} else {
		rc = 2;
	}
	
	if (rc > 1) { // o eroare -- eliberam buffer-ul
		printf("Eliberam buffer-ul...\n");
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
int cgym_send_get_req(cgym_sock_t *sock, cgym_segment_t *s) {
	int rc = 0;
	char *buf, *filename;
	
	if (sock != NULL) {
		if (s != NULL) {
			filename = cgym_entry_file( cgym_segment_entry(s) );
			
			if (filename != NULL) {
				if ((buf = malloc(strlen(CGYM_GET_MSG)
								+ strlen(filename) + 1)) != NULL) {
					sprintf(buf, CGYM_GET_MSG, filename);

					do {
						rc = cgym_send(sock, buf, strlen(buf));
					} while (rc == 1); // inca nu a terminat
					
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
int cgym_recv_get_reply(cgym_sock_t *sock, cgym_segment_t *s) {
	int rc = cgym_recv(sock, 30);
	
	return 0;
}
