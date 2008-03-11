#ifndef LIBCGYM_PRIV_H_
#define LIBCGYM_PRIV_H_
struct cgym_server_t_ {
	char *addr;
	unsigned int port;
};

struct cgym_sock_t_ {
        int sockfd;
        enum cgym_sock_state state;
        /* NONE, IDLE, CONNECTING, RECV_SIZE, RECV_DATA, ... */

		cgym_server_t *server;

        char *buf;
        long pos, capacity;
};

struct cgym_entry_t_ {
        enum cgym_entry_type type; /* NONE, FILE, DIRECTORY */
        unsigned long size; /* 0 for directory */
        char *md5; /* NULL for directory */
        char *file;
};

struct cgym_segment_t_ {
		cgym_sock_t *sock;
        cgym_entry_t *entry;
        unsigned long start;
        unsigned long stop;
};

/* Functii interne */

int cgym_recv_handshake(cgym_sock_t *sock);
#endif /*LIBCGYM_PRIV_H_*/
