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

cgym_sock_t *cgym_sock_create(cgym_server_t *server) {
	cgym_sock_t *rc = NULL;
	
	if (server != NULL) {
		if ((rc = malloc(sizeof(*rc))) != NULL) {
			rc->state = CGYM_SOCK_IDLE;
			rc->server = server;
			
			rc->buf = NULL;
			rc->pos = 0;
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

int cgym_sock_clear(cgym_sock_t *sock) {
	int rc = 0;
	
	if (sock != NULL) {
		if (sock->buf != NULL) {
			free(sock->buf);
			sock->capacity = 0;
		}
	} else {
		rc = 1;
	}
	
	return rc;
}

int cgym_sock_get_sockfd(cgym_sock_t *sock) {
	int rc = -1;
	
	if (sock != NULL) {
		rc = sock->sockfd;
	}
	
	return rc;
}

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

void cgym_sock_print_info(cgym_sock_t *sock) {
	char *ptr;
	
	if (sock != NULL) {
		printf("[%d]%s:%d  ",
			sock->sockfd, sock->server->addr, sock->server->port);
		switch (sock->state) {
			case CGYM_SOCK_NONE: ptr = "NONE"; break;
			case CGYM_SOCK_IDLE: ptr = "IDLE"; break;
			case CGYM_SOCK_CONNECTING: ptr = "CONNECTING"; break;
			case CGYM_SOCK_RECV_SIZE: ptr = "RECV_SIZE"; break;
			case CGYM_SOCK_RECV_DATA: ptr = "RECV_DATA"; break;
			default: ptr = "Unknown state"; break;
		}
		printf("%s (%d)", ptr, sock->state);
	} else {
		printf("(null)");
	}
}

void cgym_sock_free(cgym_sock_t *sock) {
	if (sock != NULL) {
		if (sock->state == CGYM_SOCK_RECV_SIZE
				|| sock->state == CGYM_SOCK_RECV_DATA) { // connected
			close(sock->sockfd);
		}
		
		cgym_server_info_free(sock->server);
		cgym_sock_clear(sock);
		free(sock);
	}
}

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

int cgym_sock_connect(cgym_sock_t *sock) {
	int rc = 0, optval;
	struct hostent *he;
    struct sockaddr_in their_addr; // connector's address information
    socklen_t lon = sizeof(optval);
	
	if ((he=gethostbyname(sock->server->addr)) != NULL) {  // get the host info
		their_addr.sin_family = AF_INET;    // host byte order 
		their_addr.sin_port = htons(sock->server->port);  // short, network byte order 
		their_addr.sin_addr = *((struct in_addr *)he->h_addr);
		memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);
	
		if (sock != NULL) {
			if (sock->state == CGYM_SOCK_IDLE) {
				rc = connect(sock->sockfd, (struct sockaddr *)&their_addr,
															sizeof their_addr);
			
				if (rc == -1) {
					if (errno == EINPROGRESS) {
						// connect() in progress
						rc = 1;
					} else {
						// eroare
						perror("connect");
						rc = 2;
					}
				}
			} else if (sock->state == CGYM_SOCK_CONNECTING) {
				if (getsockopt(sock->sockfd, SOL_SOCKET,
								SO_ERROR, (void*)(&optval), &lon) < 0) { 
					perror("getsockopt");
					rc = 3;
				} else if (optval != 0) {
					rc = 4;
				}
			} else { // unknown state
				rc = 5;
			}
		} else {
			rc = 6;
		}
	} else {
		rc = 7;
	}
	
	return rc;
}

long cgym_recv(cgym_sock_t *sock, long len) {
	if (sock->pos + len < sock->capacity) {
		
	}
}
