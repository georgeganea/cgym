#ifndef LIBCGYM_H_
#define LIBCGYM_H_
#include <stdio.h>

#define CGYM_DEFAULT_PORT 2496
#define CGYM_ACK_MESSAGE "CGYM\r\n"
#define CGYM_ERR_MESSAGE "ERR\r\n"

typedef struct cgym_server_t_ cgym_server_t;

/*
 * Extract server address and port number from the string
 */
 cgym_server_t *cgym_server_info_create(char *server);
 void cgym_server_info_free(cgym_server_t *server);
 void cgym_server_info_print(cgym_server_t *server);
  

enum cgym_sock_state {
	CGYM_SOCK_NONE,
	CGYM_SOCK_IDLE,
	CGYM_SOCK_CONNECTING,
	CGYM_SOCK_RECV_HANDSHAKE,
	CGYM_SOCK_CONNECTED,
	CGYM_SOCK_RECV_LIST,
	CGYM_SOCK_RECV_SIZE,
	CGYM_SOCK_RECV_DATA,
	
	CGYM_SOCK_ERR
};
typedef struct cgym_sock_t_ cgym_sock_t;

cgym_sock_t *cgym_sock_create(cgym_server_t *server);
int cgym_sock_clear(cgym_sock_t *sock);
int cgym_sock_get_sockfd(cgym_sock_t *sock);
int cgym_sock_get_state(cgym_sock_t *sock);
cgym_server_t *cgym_sock_get_server(cgym_sock_t *sock);
void cgym_sock_print_info(cgym_sock_t *sock);
void cgym_sock_free(cgym_sock_t *sock);


enum cgym_entry_type {
	CGYM_ENTRY_NONE,
	CGYM_ENTRY_FILE,
	CGYM_ENTRY_DIRECTORY
};
typedef struct cgym_entry_t_ cgym_entry_t;

cgym_entry_t *cgym_entry_init(char *file, char *md5,
							int type, unsigned long size);
int cgym_entry_type(cgym_entry_t *e);
unsigned long cgym_entry_size(cgym_entry_t *e);
char *cgym_entry_md5(cgym_entry_t *e);
char *cgym_entry_file(cgym_entry_t *e);
void cgym_entry_free(cgym_entry_t *e);


typedef struct cgym_segment_t_ cgym_segment_t;

cgym_segment_t *cgym_segment_init(cgym_sock_t *s, cgym_entry_t *e,
								unsigned long start, unsigned long stop);
cgym_sock_t *cgym_segment_sock(cgym_segment_t *s);
int cgym_segment_set_sock(cgym_segment_t *s, cgym_sock_t *sock);
cgym_entry_t *cgym_segment_entry(cgym_segment_t *s);
unsigned long cgym_segment_start(cgym_segment_t *s);
unsigned long cgym_segment_stop(cgym_segment_t *s);
void cgym_segment_free(cgym_segment_t *s);

/* CGYM client */
int cgym_send_quit(cgym_sock_t *sock);

/* CGYM client -- LIST */
int cgym_send_list_req(cgym_sock_t *sock, char *dir);
int cgym_recv_list_reply(cgym_sock_t *sock, cgym_entry_t **e);
int cgym_print_list(char *dir, cgym_entry_t **e);

/* CGYM client -- GET */
int cgym_sock_setblocking(cgym_sock_t *sock, int block);
int cgym_sock_connect(cgym_sock_t *sock);

int cgym_send_size_req(cgym_sock_t *sock, char *file);
int cgym_recv_size_reply(cgym_sock_t *sock, cgym_entry_t **e);

int cgym_send_get_req(cgym_sock_t *sock, cgym_segment_t *s);
int cgym_recv_get_reply(cgym_sock_t *sock, cgym_segment_t *s);

int cgym_segment_assemble(FILE *fd, cgym_segment_t **s);

/*MD5 functions*/

char* compute_md5(char* filename);
void MD5Print (unsigned char digest[16]);

#endif /*LIBCGYM_H_*/
