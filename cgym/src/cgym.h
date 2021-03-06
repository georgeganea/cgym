#ifndef CGYM_H_
#define CGYM_H_
#include "libcgym.h"

/*
 * Lists the contents of the specified path on the specified server.
 */
int cgym_list(cgym_server_t *server, char *path);

/*
 * Gets the specified file from the specified servers, using n segments.
 */
int cgym_get(char *remote, int segments, cgym_server_t **servers);

/*
 * Extract the file name from the full path (path using '/' slashes)
 */
char *cgym_get_file_name(char *file);

#endif /*CGYM_H_*/
