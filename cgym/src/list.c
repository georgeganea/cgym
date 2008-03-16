#include "cgym.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "libcgym_priv.h"
#define MAXDATASIZE 100



int cgym_list(cgym_server_t *s, char *path) {
	printf("creating socket and connecting to server\n");
	cgym_sock_t *my_socket =  cgym_sock_create(s);
	if (cgym_sock_connect(my_socket)==0){
		sleep(1);
	cgym_send_list_req(my_socket,path);
	
	}

	cgym_entry_t **e;
	cgym_recv_list_reply(my_socket,&e);
	printf("requesting list of directory %s from\n", path);
	cgym_server_info_print(s);
	printf("\n");
	
	cgym_print_list(e);

return 0;
}

