#include "libcgym.h"
#include "libcgym_priv.h"

int cgym_send_size_req(cgym_sock_t *sock, char *file);
int cgym_recv_size_reply(cgym_sock_t *sock, cgym_entry_t **e);

int cgym_send_get_req(cgym_sock_t *sock, cgym_segment_t *s);
int cgym_recv_get_reply(cgym_sock_t *sock, cgym_segment_t *s);
