#include <stdlib.h>
#include <string.h>
#include "libcgym.h"
#include "libcgym_priv.h"

/*
 * aloca resurse pentru un entry si salveaza numele acestuia
 *
 * returneaza entry-ul la succes, NULL la eroare
 */
cgym_entry_t *cgym_entry_init(char *file, char *md5,
							int type, unsigned long size) {
	cgym_entry_t *rc = malloc(sizeof(*rc));
	
	if (rc != NULL) {
		if (file != NULL) {
			rc->file = malloc(strlen(file)+1);
		
			if (rc->file != NULL) {
				strcpy(rc->file, file);
			} else {
				free(rc);
				return NULL;
			}
		}
		
		if (md5 != NULL) {
			rc->md5 = malloc(strlen(md5)+1);
			
			if (rc->md5 != NULL) {
				strcpy(rc->md5, md5);
			} else {
				if (rc->file)
					free(rc->file);
				free(rc);
				return NULL;
			}
		}
		
		rc->type = type;
		rc->size = size;
	}
	
	return rc;
}


/*
 * returneaza tipul entry-ului { NONE, FILE, DIRECTORY }
 */
int cgym_entry_type(cgym_entry_t *e) {
	int rc = CGYM_ENTRY_NONE;
	
	if (e != NULL) {
		rc = e->type;
	}
	
	return rc;
}

/*
 * returneaza campul size al entry-ului
 */
unsigned long cgym_entry_size(cgym_entry_t *e) {
	unsigned long rc = -1;
	
	if (e != NULL) {
		rc = e->size;
	}
	
	return rc;
}

/*
 * returneaza campul MD5 al entry-ului
 */
char *cgym_entry_md5(cgym_entry_t *e) {
	char *rc = NULL;
	
	if (e != NULL) {
		rc = e->md5;
	}
	
	return rc;
}

/*
 * returneaza campul file al entry-ului
 */
char *cgym_entry_file(cgym_entry_t *e) {
	char *rc = NULL;
	
	if (e != NULL) {
		rc = e->file;
	}
	
	return rc;
}

/*
 * elibereaza resursele entry-ului
 * returneaza 0 la succes, nenul la eroare
 */
void cgym_entry_free(cgym_entry_t *e) {
	if (e != NULL) {
		free(e->file);
		free(e->md5);
		
		free(e);
	}
}
