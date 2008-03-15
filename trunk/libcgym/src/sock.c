#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
//#include <netinet/in.h>
#include "libcgym.h"
#include "libcgym_priv.h"

/*
 * creeaza un nou socket (blocking) care se va
 * conecta la server/port cand se apeleaza cgym_sock_connect()
 *
 * returneaza socket-ul la succes, NULL la eroare
 */
cgym_sock_t *cgym_sock_create(cgym_server_t *server) {
	cgym_sock_t *rc = NULL;
	
	if (server != NULL) {
		if ((rc = malloc(sizeof(*rc))) != NULL) {
			rc->state = CGYM_SOCK_IDLE;
			rc->server = server;
			
			rc->buf = NULL;
			rc->pos_recv = 0;
			rc->capacity = 0;
			
		    if ((rc->sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
       			perror("socket");
    			free(rc);
   				rc = NULL;
   			}
		}
	}
	
	return rc;
}

/*
 * sterge continutul bufferului intern
 * returneaza 0 la succes, nenul la eroare
 */
int cgym_sock_clear(cgym_sock_t *sock) {
	int rc = 0;
	
	if (sock != NULL) {
		sock->capacity = 0;
		sock->pos_recv = 0;
		sock->pos_send = 0;
		if (sock->buf != NULL) {
			free(sock->buf);
			sock->buf = NULL;
		}
	} else {
		rc = 1;
	}
	
	return rc;
}

/*
 * returneaza valoarea campului sockfd
 */
int cgym_sock_get_sockfd(cgym_sock_t *sock) {
	int rc = -1;
	
	if (sock != NULL) {
		rc = sock->sockfd;
	}
	
	return rc;
}

/*
 * returneaza valoarea campului state
 */
int cgym_sock_get_state(cgym_sock_t *sock) {
	enum cgym_sock_state rc = CGYM_SOCK_NONE;
	
	if (sock != NULL) {
		rc = sock->state;
	}
	
	return rc;
}

cgym_server_t *cgym_sock_get_server(cgym_sock_t *sock) {
	cgym_server_t *rc = NULL;
	
	if (sock != NULL) {
		rc = sock->server;
	}
	
	return rc;
}

/*
 * afiseaza la stdout informatii despre sock
 */
void cgym_sock_info(cgym_sock_t *sock) {
	char *ptr;
	
	if (sock != NULL) {
		printf("[%d]%s:%d  ",
			sock->sockfd, sock->server->addr, sock->server->port);
		switch (sock->state) {
			case CGYM_SOCK_NONE: ptr = "NONE"; break;
			case CGYM_SOCK_IDLE: ptr = "IDLE"; break;
			case CGYM_SOCK_CONNECTING: ptr = "CONNECTING"; break;
			case CGYM_SOCK_RECV_HANDSHAKE: ptr = "RECV_HANDSHAKE"; break;
			case CGYM_SOCK_CONNECTED: ptr = "CONNECTED"; break;
			case CGYM_SOCK_RECV_LIST: ptr = "RECV_LIST"; break;
			case CGYM_SOCK_RECV_SIZE_REPLY: ptr = "RECV_SIZE_REPLY"; break;
			case CGYM_SOCK_RECV_SIZE_DATA: ptr = "RECV_SIZE_DATA"; break;
			case CGYM_SOCK_RECV_DATA: ptr = "RECV_DATA"; break;
			
			case CGYM_SOCK_ERR: ptr = "ERR"; break;
			default: ptr = "Unknown state"; break;
		}
		printf("%s (%d)", ptr, sock->state);
		
		printf("buf[cap=%ld,pos=%ld,s=%p]",
				sock->capacity, sock->pos_recv, (void *)sock->buf);
	} else {
		printf("(null)");
	}
}

/*
 * inchide socket-ul si elibereaza resursele
 * 
 * returneaza 0 la succes, nenul la eroare
 */
void cgym_sock_free(cgym_sock_t *sock) {
	if (sock != NULL) {
		if (sock->state != CGYM_SOCK_NONE
				&& sock->state != CGYM_SOCK_IDLE) { // connected
			close(sock->sockfd);
		}
		
		cgym_server_info_free(sock->server);
		cgym_sock_clear(sock);
		free(sock);
	}
}

/*
 * pune socket-ul dat in mod blocking daca block este 1,
 * sau nonblocking daca este 0.
 *
 * returneaza 0 la succes, nenul la eroare
 */
int cgym_sock_setblocking(cgym_sock_t *sock, int block) {
	int flags, rc = 0;
	
	if (sock != NULL) {
		if ((rc = fcntl(sock->sockfd, F_GETFL, 0)) != -1) {
			if (block) {
				/* Set socket to blocking */
				if (fcntl(sock->sockfd, F_SETFL, flags & (~O_NONBLOCK)) < 0) {
					return -1;
				} 
			} else {
				/* Set socket to non-blocking */
				if (fcntl(sock->sockfd, F_SETFL, flags | O_NONBLOCK) < 0)
				{
					return -1;
				}
			}
		} else {
			rc = 1;
		}
	} else {
		rc = 2;
	}
	
	return rc;
}

/*
 * conecteaza socket-ul la adresa/portul dat
 * 
 * returneaza:
 * 	0 la succes
 *	1 la incomplet
 *	2 la eroare la connect
 *	3 la eroare la gethostbyname
 *	4 la eroare la getsockopt
 *	5 la eroare - nu s-a putut conecta
 *	6 la eroare la cgym_recv_handshake
 *	7 la eroare - sock-ul e NULL
 */
int cgym_sock_connect(cgym_sock_t *sock) {
	int rc = 0, optval;
	struct hostent *he;
    struct sockaddr_in their_addr; // connector's address information
    socklen_t lon = sizeof(optval);
    
	if (sock != NULL) {
		if (sock->state == CGYM_SOCK_IDLE) {
			if ((he=gethostbyname(sock->server->addr)) != NULL) {  // get the host info
				their_addr.sin_family = AF_INET;    // host byte order 
				their_addr.sin_port = htons(sock->server->port);  // short, network byte order 
				their_addr.sin_addr = *((struct in_addr *)he->h_addr);
				memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);
			
				rc = connect(sock->sockfd, (struct sockaddr *)&their_addr,
															sizeof their_addr);
			
				if (rc == -1) {
					if (errno == EINPROGRESS) {
						// connect() in progress
						sock->state = CGYM_SOCK_CONNECTING;
						rc = 1;
					} else {
						// eroare
						perror("connect");
						sock->state = CGYM_SOCK_ERR;
						rc = 2;
					}
				} else { // connected!
					sock->state = CGYM_SOCK_RECV_HANDSHAKE;
				}
			} else {
				sock->state = CGYM_SOCK_ERR;
				rc = 3;
			}
		} else if (sock->state == CGYM_SOCK_CONNECTING) {
			if (getsockopt(sock->sockfd, SOL_SOCKET,
							SO_ERROR, (void*)(&optval), &lon) < 0) { 
				perror("getsockopt");
				sock->state = CGYM_SOCK_ERR;
				rc = 4;
			} else if (optval != 0) {
				sock->state = CGYM_SOCK_ERR;
				rc = 5;
			}
		}
		
		if (!rc && sock->state == CGYM_SOCK_RECV_HANDSHAKE) {
			rc = cgym_recv_handshake(sock);
			
			if (rc == 0) { // am reusit!
				sock->state = CGYM_SOCK_CONNECTED;
			} else if (rc > 1) { // eroare
				sock->state = CGYM_SOCK_ERR;
				rc = 6;
			}
		}
	} else {
		rc = 7;
	}
	
	return rc;
}

/* returneaza:
 * 	0 la succes
 * 	1 la incomplet
 * 	2 la remote closed connection
 * 	3 la eroare la recv()
 * 	4 la eroare NULL la realloc
 * 	5 la sock == NULL
 */
int cgym_recv(cgym_sock_t *sock, unsigned long len) {
	int rc = 0;
	char *tmp;
	
	if (sock != NULL) {
		if (len > sock->capacity) {
			tmp = realloc(sock->buf, len);
			if (tmp != NULL) {
				sock->buf = tmp;
				sock->capacity = len;				
			} else {
				free(sock->buf);
				sock->buf = NULL;
				
				sock->pos_recv = 0;
				sock->capacity = 0;
				
				rc = 4;
			}
			
			if (!rc) {
				rc = recv(sock->sockfd,
						sock->buf + sock->pos_recv, len - sock->pos_recv, 0);
				
				printf("received %d: %c%c...\n",
							rc,
							sock->buf[sock->pos_recv],
							sock->buf[sock->pos_recv+1]);
				
				if (rc == len - sock->pos_recv) {
					//sock->pos_recv += rc;
					sock->pos_recv = 0;
					rc = 0; // gata
				} else if (rc > 0) {
					// mai avem de primit
					sock->pos_recv += rc;
					rc = 1;
				} else if (rc == 0) {
					// s-a inchis conexiunea
					rc = 2;
				} else if (rc < 0) {
					if (errno == EAGAIN) {
						rc = 1;
					} else {
						// eroare
						rc = 3;
					}
				}
			}
		}
	} else {
		rc = 5;
	}
	
	printf("end of sock_recv(): ");
	cgym_sock_info(sock);
	printf("\n");
	
	return rc;
}

/*
 * trimite mesajul buf de len octeti pe sock
 * returneaza:
 * 
 * 	0 la succes
 * 	1 la incomplet
 * 	2 la eroare la send()
 * 	3 la eroare - un apel, cand fosta trimitere nu s-a terminat
 * 	4 la eroare - sock este NULL
 */
int cgym_send(cgym_sock_t *sock, char *buf, unsigned long len) {
	int rc = 0;
	
	if (sock != NULL) {
		if (sock->pos_send < len) {
				rc = send(sock->sockfd,
						buf + sock->pos_send, len - sock->pos_send, 0);
				
				if (rc == len - sock->pos_send) {
					// am trimis tot
					sock->pos_send = 0;
					rc = 0;
				} else if (rc > 0) {
					// am trimis o parte
					sock->pos_send += rc;
					rc = 1;
				} else if (rc < 0) {
					if (errno == EAGAIN) {
						// would block, try again
						rc = 1;
					} else {
						// some real error
						rc = 2;
					}
				}
		} else { // functia nu este folosita cum trebuie
			rc = 3;
		}
	} else 	{
		return 4;
	}
	
	return rc;
}

/* 
 * Folosita de cgym_sock_connect()
 * 
 * Returneaza

 * 	0 la succes
 * 	1 la incomplet
 * 	>1 la eroare
 */
int cgym_recv_handshake(cgym_sock_t *sock) {
	int rc = cgym_recv(sock, strlen(CGYM_ACK_MSG));
	
	if (rc == 0) {
		// am primit tot
		if (strncmp(sock->buf, CGYM_ACK_MSG, strlen(CGYM_ACK_MSG))) {
			// nu e bun
			rc = 2;
		} else {
			cgym_sock_clear(sock);
		}
	} else if (rc > 1) {
		// eroare
		rc = 3;
	}
	
	return rc;
}

/*
 * Trimite mesajul de deconectare la server pe sock
 * 
 * Returneaza
 * 	0 la succes
 * 	2 la eroare
 */
int cgym_send_quit(cgym_sock_t *sock) {
	int rc = 0;
	
	if (sock != NULL) {
		do {
			rc = cgym_send(sock, CGYM_QUIT_MSG, strlen(CGYM_QUIT_MSG));
		} while (rc == 1); // inca nu a terminat
	} else {
		rc = 2;
	}
	
	return rc;
}
