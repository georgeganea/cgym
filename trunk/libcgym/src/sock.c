#include "libcgym.h"

struct cgym_sock_t_ {
        int sockfd;
        enum cgym_sock_state state; /* IDLE, CONNECTING, RECV_SIZE, RECV_DATA */

        char *server;
        int port;

        char *buf;
};

cgym_sock_t *cgym_sock_create(char *server, int port) {
	return NULL;
}

int cgym_sock_clear(cgym_sock_t *sock) {
	return 0;
}

int cgym_sock_sockfd(cgym_sock_t *sock) {
	return 0;
}

int cgym_sock_state(cgym_sock_t *sock) {
	return 0;
}

void cgym_sock_info(cgym_sock_t *sock) {
}

int cgym_sock_free(cgym_sock_t **sock) {
	return 0;
}
