#include <stdlib.h>
#include "libcgym.h"
#include "libcgym_priv.h"

cgym_entry_t *cgym_entry_init(char *file) {
	cgym_entry_t *rc = malloc(sizeof(*rc));
	
	if (rc != NULL) {
		
	}
	
	return rc;
}

int cgym_entry_type(cgym_entry_t *e);
unsigned long cgym_entry_size(cgym_entry_t *e);
char *cgym_entry_md5(cgym_entry_t *e);
char *cgym_entry_file(cgym_entry_t *e);
void cgym_entry_free(cgym_entry_t *e);
