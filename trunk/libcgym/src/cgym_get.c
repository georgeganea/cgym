#include "libcgym.h"

int cgym_sock_setblocking(cgym_sock_t *sock, int block);
int cgym_sock_connect(cgym_sock_t *sock);

int cgym_send_size_req(cgym_sock_t *sock, char *file);
int cgym_recv_size_reply(cgym_sock_t *sock, cgym_entry_t **e);

int cgym_send_get_req(cgym_sock_t *sock, cgym_segment_t *s);
int cgym_recv_get_reply(cgym_sock_t *sock, cgym_segment_t *s);

int cgym_segment_assemble(FILE *fd, cgym_segment_t **s);
