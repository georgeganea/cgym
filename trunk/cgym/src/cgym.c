#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cgym.h"

int main(int argc, char **argv) {
	char *path;
	
	if (argc < 2) {
		fprintf(stderr, "usage: cgym list [DIRECTORY] server\n"
		                "            get [-n N] file server1 ... serverN\n");
		return 1; 
	}
	
	if (!strcmp(argv[1], "list")) {
		cgym_server_t *s;
		
		if (argc < 3 || argc > 4) {
			fprintf(stderr, "usage: cgym list [DIRECTORY] server\n");
			return 1;
		}
		
		s = cgym_get_server_info(argv[argc-1]);
		
		if (argc == 4) // directory argument present
			path = argv[2];
		else
			path = "/";
			
		cgym_list(s, path);
		
		free(s->addr);
		free(s);
	} else if (!strcmp(argv[1], "get")) {
		cgym_server_t **slist;
		char *tail, *local;
		int i, start = 2, segments = CGYM_SEGMENT_DEFAULT;
		
		if (argc < 4) {
			fprintf(stderr, "usage: cgym get [-n N] file server1 ... serverN\n");
			return 1;
		}
		
		if (!strcmp(argv[2], "-n")) { // trebuie sa avem "-n"				
			i = strtol(argv[3], &tail, 10);
			
			if (*tail == '\0') { // ce vine dupa "-n" este numar
				if (argc < 6) { // dar nu sunt destule argumente sa fie optiune
					fprintf(stderr, "usage: cgym get [-n N] file server1 ... serverN\n");
					return 1;
				}
				
				start = 4;
				segments = i;
			} // else, the -n is the file name (weird, huh?)
		}
		
		path = argv[start++];
		
		slist = malloc(sizeof(*slist) * (argc - start + 1));
		
		for (i = start; i < argc; i++)
			slist[i - start] = cgym_get_server_info(argv[i]);
		slist[argc - start] = NULL;
		
		local = cgym_get_file_name(path);
		
		cgym_get(path, local, segments, slist);
		
		free(local);
		
		for (i = start; i < argc; i++) {
			free(slist[i - start]->addr);
			free(slist[i - start]);
		}

		free(slist);
	} else {
		fprintf(stderr, "error: operation must be either 'list' or 'get'.\n");
		return 1;
	}
	
	return 0;
}

cgym_server_t *cgym_get_server_info(char *server) {
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

char *cgym_get_file_name(char *file) {
	char *ptr, *rc = NULL;
	
	if ((ptr = strrchr(file, '/')) == NULL) {
		ptr = file;
	} else {
		ptr++;
	}
	
	rc = malloc(strlen(ptr) + 1);
		
	if (rc != NULL) {
		strcpy(rc, ptr);
	}
	
	return rc;
}
