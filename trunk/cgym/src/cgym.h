#ifndef CGYM_H_
#define CGYM_H_

#define CGYM_DEFAULT_PORT 2496
#define CGYM_SEGMENT_DEFAULT 1

typedef struct {
	char *addr;
	unsigned int port;
} cgym_server_t;

/*
 * Lists the contents of the specified path on the specified server.
 */
int cgym_list(cgym_server_t *server, char *path);

/*
 * Gets the specified file from the specified servers, using n segments.
 */
int cgym_get(char *remote, char *local, int segments, cgym_server_t **servers);

/*
 * !! Should probably be moved to the libcgym
 */

/*
 * Extract server address and port number from the string
 */
 cgym_server_t *cgym_get_server_info(char *server);
 /*
  * Extract the file name from the full path (path using '/' slashes)
  */
 char *cgym_get_file_name(char *file);
#endif /*CGYM_H_*/
