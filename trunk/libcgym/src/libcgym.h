#ifndef LIBCGYM_H_
#define LIBCGYM_H_
#include <stdio.h>

#define CGYM_DEFAULT_PORT 2496

typedef struct cgym_server_t_ cgym_server_t;

/*
 * Extract server address and port number from the string
 */
 cgym_server_t *cgym_server_info_create(char *server);
 void cgym_server_info_free(cgym_server_t *server);
 void cgym_server_info_print(cgym_server_t *server);
  

enum cgym_sock_state {
	CGYM_IDLE, CGYM_CONNECTING, CGYM_RECV_SIZE, CGYM_RECV_DATA
};
typedef struct cgym_sock_t_ cgym_sock_t;

cgym_sock_t *cgym_sock_create(char *server, int port);
int cgym_sock_clear(cgym_sock_t *sock);
int cgym_sock_sockfd(cgym_sock_t *sock);
int cgym_sock_state(cgym_sock_t *sock);
void cgym_sock_info(cgym_sock_t *sock);
void cgym_sock_free(cgym_sock_t *sock);


enum cgym_entry_type { CGYM_NONE, CGYM_FILE, CGYM_DIRECTORY };
typedef struct cgym_entry_t_ cgym_entry_t;

cgym_entry_t *cgym_entry_init(char *file);
int cgym_entry_type(cgym_entry_t *e);
unsigned long cgym_entry_size(cgym_entry_t *e);
char *cgym_entry_md5(cgym_entry_t *e);
char *cgym_entry_file(cgym_entry_t *e);
void cgym_entry_free(cgym_entry_t *e);


typedef struct cgym_segment_t cgym_segment_t;

cgym_segment_t *cgym_segment_init(cgym_entry_t *e, unsigned long start, unsigned long stop);
cgym_entry_t *cgym_segment_entry(cgym_segment_t *s);
unsigned long cgym_segment_start(cgym_segment_t *s);
unsigned long cgym_segment_stop(cgym_segment_t *s);
void cgym_segment_free(cgym_segment_t *s);

/* CGYM client -- LIST */
int cgym_list_get(cgym_sock_t *sock, char *dir, cgym_entry_t **e);
int cgym_list_print(char *dir, cgym_entry_t **e);

/* CGYM client -- GET */
int cgym_sock_setblocking(cgym_sock_t *sock, int block);
int cgym_sock_connect(cgym_sock_t *sock);

int cgym_send_size_req(cgym_sock_t *sock, char *file);
int cgym_recv_size_reply(cgym_sock_t *sock, cgym_entry_t **e);

int cgym_send_get_req(cgym_sock_t *sock, cgym_segment_t *s);
int cgym_recv_get_reply(cgym_sock_t *sock, cgym_segment_t *s);

int cgym_segment_assemble(FILE *fd, cgym_segment_t **s);
#endif /*LIBCGYM_H_*/
