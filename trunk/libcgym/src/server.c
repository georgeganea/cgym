#include <stdlib.h>
#include <string.h>
#include "libcgym.h"

struct cgym_server_t_ {
	char *addr;
	unsigned int port;
};


cgym_server_t *cgym_server_info_create(char *server) {
	char *ptr;
	cgym_server_t *rc = NULL;
	
	if (server != NULL) {
		rc = malloc(sizeof(*rc));
		
		if (rc == NULL)
			return rc;
		
		if ((ptr = strchr(server, ':')) != NULL) { // server:port
			rc->addr = malloc(ptr - server + 1);
			
			if (rc->addr == NULL) {
				free(rc);
				return NULL;
			}
			
			memcpy(rc->addr, server, ptr - server);
			rc->addr[ptr - server] = '\0';
			
			server = ptr + 1;
			rc->port = strtol(server, &ptr, 10);
			
			if (*ptr != '\0') { // not a valid port number
				free(rc->addr);
				free(rc);
				return NULL;
			}
		} else { // server (default port)
			rc->addr = malloc(strlen(server) + 1);
			
			if (rc->addr == NULL) {
				free(rc);
				return NULL;
			}
			
			strcpy(rc->addr, server);
			rc->port = CGYM_DEFAULT_PORT;
		}
	}
	
	return rc;
}

void cgym_server_info_free(cgym_server_t *server) {
	if (server != NULL) {
		free(server->addr);
		free(server);
	}
}

void cgym_server_info_print(cgym_server_t *server) {
	if (server != NULL) {
		printf("%s:%d", server->addr, server->port);
	} else {
		printf("(null)");
	}
}
