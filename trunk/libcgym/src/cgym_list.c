#include "libcgym.h"

/*
 * trimite cererea pentru lista catre server
 * returneaza:
 * 	0 la succes
 * 	1 la eroare
 */
int cgym_send_list_req(cgym_sock_t *sock, char *dir);

/*
 * citeste de pe socket lista trimisa de server
 * si o salveaza in e
 * tabelul se termina cu NULL
 *
 * returneaza:
 *	0 la succes
 *	1 la incomplet
 *	2 la eroare 
 */
int cgym_recv_list_reply(cgym_sock_t *sock, cgym_entry_t **e);

/*
 * afiseaza lista de entry-uri la stdout
 *
 * returneaza numarul de entry-uri
 */
int cgym_print_list(char *dir, cgym_entry_t **e);
