
```
struct {
	int sockfd;
	int state; /* IDLE, CONNECTING, RECV_SIZE, RECV_DATA */

	char *server;
	int port;

	char *buf;
} cgym_sock_t;

/*
 * creeaza un nou socket (blocking) care se va
 * conecta la server/port cand se apeleaza cgym_sock_connect()
 *
 * returneaza socket-ul la succes, NULL la eroare
 */
cgym_sock_t *cgym_sock_create(char *server, int port)

/*
 * sterge continutul bufferului intern
 * returneaza 0 la succes, nenul la eroare
 */
int cgym_sock_clear(cgym_sock_t *sock)

/*
 * returneaza valoarea campului sockfd
 */
int cgym_sock_sockfd(cgym_sock_t *sock)

/*
 * returneaza valoarea campului state
 */
int cgym_sock_state(cgym_sock_t *sock)

/*
 * afiseaza la stdout "(sockfd)server:port"
 */
void cgym_sock_info(cgym_sock_t *sock)

/*
 * inchide socket-ul si elibereaza resursele
 * returneaza 0 la succes, nenul la eroare
 * la succes, pune sock pe NULL
 */
int cgym_sock_free(cgym_sock_t **sock)
```