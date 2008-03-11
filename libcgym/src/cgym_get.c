#include "libcgym.h"
#include "libcgym_priv.h"

/*
 * trimite cererea pentru informatia despre fisier
 *
 * returneaza 0 la succes si nenul la eroare
 */
int cgym_send_size_req(cgym_sock_t *sock, char *file);

/*
 * citeste de pe socket informatia despre fisier
 * trimisa de server si o salveaza in e
 *
 * returneaza:
 *	0 la succes
 *	1 la incomplet
 *	2 la eroare
 */
int cgym_recv_size_reply(cgym_sock_t *sock, cgym_entry_t **e);

/*
 * trimite cererea pentru continutul fisierului
 *
 * returneaza 0 la succes si nenul la eroare
 */
int cgym_send_get_req(cgym_sock_t *sock, cgym_segment_t *s);

/*
 * citeste de pe socket raspunsul serverului si
 * continutul fisierului si il salveaza in s
 *
 * returneaza:
 *	0 la succes
 *	1 la incomplet
 *	2 la eroare
 */
int cgym_recv_get_reply(cgym_sock_t *sock, cgym_segment_t *s);
