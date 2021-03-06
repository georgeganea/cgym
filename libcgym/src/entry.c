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

cgym_entry_t *cgym_entry_init_raw(char *str) {
	enum cgym_entry_type type = CGYM_ENTRY_NONE;
	unsigned long size;
	char md5[MD5_STR_LEN+1], file[MAX_LINE_LEN+1], type_c, tmp[25];
	cgym_entry_t *rc = NULL;
	
	if (str != NULL) {
		sprintf(tmp, "%%c %%ld %%%ds %%%ds\r\n",
					MD5_STR_LEN, MAX_LINE_LEN);
		
		if (sscanf(str, tmp, &type_c, &size, md5, file) == 4) {
			switch (type_c) {
				case '-': type = CGYM_ENTRY_FILE; break;
				case 'd': type = CGYM_ENTRY_DIRECTORY; break;
			}
			
			rc = cgym_entry_init(file, md5, type, size);
		} // else eroare -- formatul e gresit
	}
	
	return rc;
}

void cgym_entry_info(cgym_entry_t *e) {
	if (e != NULL) {
		char *tmp;
		
		switch (e->type) {
			case CGYM_ENTRY_DIRECTORY: tmp="DIRECTORY"; break;
			case CGYM_ENTRY_FILE: tmp="FILE"; break;
			case CGYM_ENTRY_NONE: tmp="NONE"; break;
			default: tmp="Unknown type";
		}
		
		printf("entry:[name=%s,len=%ld,md5=%s,type=%s]\n",
				e->file, e->size, e->md5, tmp);
	} else {
		printf("(null)\n");
	}
}

/*
 * returneaza tipul entry-ului { NONE, FILE, DIRECTORY }
 */
enum cgym_entry_type cgym_entry_type(cgym_entry_t *e) {
	enum cgym_entry_type rc = CGYM_ENTRY_NONE;
	
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

/*
 * construieste un char * din informatia
 * continuta in cgym_entry_t
 * */
char * cgym_entry_tostring(cgym_entry_t *e){
	char *tmp,*tmp2=NULL,*buf;
	if (e != NULL) {
			switch (e->type) {
				case CGYM_ENTRY_DIRECTORY:{
					tmp=malloc(strlen("d")+1);
					strcpy(tmp,"d");
					tmp2=malloc(strlen("-")+1);
					strcpy(tmp2,"-"); break;
				}
				case CGYM_ENTRY_FILE:{
					tmp=malloc(strlen("-")+1);
					strcpy(tmp,"-");
					tmp2=malloc(16*sizeof(unsigned long)+1);
					sprintf(tmp2,"%lu",e->size);break;
				}
				case CGYM_ENTRY_NONE:{
					tmp=malloc(strlen("NONE")+1);
					strcpy(tmp,"NONE"); break;
				}
				default:{
					tmp=malloc(strlen("NONE")+1);
					strcpy(tmp,"NONE");
				}
			}
			char *md5 = e->md5;
			char *file = e->file;
			buf=malloc(strlen(tmp)+strlen(tmp2)+strlen(md5)+strlen(file)+6);
			sprintf(buf,"%s %s %s %s\r\n",
							tmp,tmp2,e->md5,e->file);
		}
	return buf;
}
