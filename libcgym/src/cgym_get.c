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
	return 0;
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
	return 0;
}
