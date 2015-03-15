
```
// Ramura LIST


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
int cgym_list_get(cgym_sock_t *sock, char *dir, cgym_entry_t **e)

/*
 * afiseaza lista de entry-uri la stdout
 *
 * returneaza numarul de entry-uri
 */
int cgym_list_print(char *dir, cgym_entry_t **e)


// Ramura GET


/*
 * pune socket-ul dat in mod blocking daca block este 1,
 * sau nonblocking daca este 0.
 *
 * returneaza 0 la succes, nenul la eroare
 */
int cgym_sock_setblocking(cgym_sock_t *sock, int block)

/*
 * conecteaza socket-ul la adresa/portul dat
 * 
 * returneaza:
 * 	0 la succes
 *	1 la incomplet
 *	2 la eroare
 */
int cgym_sock_connect(cgym_sock_t *sock)

/*
 * trimite cererea pentru informatia despre fisier
 *
 * returneaza 0 la succes si nenul la eroare
 */
int cgym_send_size_req(cgym_sock_t *sock, char *file)

/*
 * citeste de pe socket informatia despre fisier
 * trimisa de server si o salveaza in e
 *
 * returneaza:
 *	0 la succes
 *	1 la incomplet
 *	2 la eroare
 */
int cgym_recv_size_reply(cgym_sock_t *sock, cgym_entry_t **e)

/*
 * trimite cererea pentru continutul fisierului
 *
 * returneaza 0 la succes si nenul la eroare
 */
int cgym_send_get_req(cgym_sock_t *sock, cgym_segment_t *s)

/*
 * citeste de pe socket raspunsul serverului si
 * continutul fisierului si il salveaza in s
 *
 * returneaza:
 *	0 la succes
 *	1 la incomplet
 *	2 la eroare
 */
int cgym_recv_get_reply(cgym_sock_t *sock, cgym_segment_t *s)

/*
 * asambleaza segmentele s si le scrie in fd
 * lista s se termina cu elementul NULL
 *
 * returneaza:
 *	0 la succes
 *	1 la eroare
 */
int cgym_segment_assemble(FILE *fd, cgym_segment_t **s)
```