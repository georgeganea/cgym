#include <stdlib.h>
#include "libcgym.h"
#include "libcgym_priv.h"
#include <string.h>

/*
 * initializeaza segmentul cu entry-ul e, octetul de inceput start (inclusiv)
 * si octetul de sfarsit stop (exclusiv)
 * returneaza segmentul nou la succes, NULL la eroare
 */
cgym_segment_t *cgym_segment_init(cgym_sock_t *s, cgym_entry_t *e,
								unsigned long start, unsigned long stop) {
	cgym_segment_t *rc = NULL;
	
	if ((rc = malloc(sizeof(*rc))) != NULL) {
		rc->sock = s;
		rc->entry = e;
		rc->start = start;
		rc->stop = stop;
		
		rc->status = CGYM_SEGMENT_IDLE;
		rc->buf = NULL;
	}
	
	return rc;
}

/*
 * returneaza valoarea campului sock
 */
cgym_sock_t *cgym_segment_sock(cgym_segment_t *s) {
	cgym_sock_t *rc = NULL;
	
	if (s != NULL) {
		rc = s->sock;
	}
	
	return rc;
}

/* modifica valoarea campului sock
 * returneaza
 * 
 * 	0 la succes
 * 	1 la eroare (s este NULL)
 */
int cgym_segment_set_sock(cgym_segment_t *s, cgym_sock_t *sock) {
	int rc = 0;
	
	if (s != NULL) {
		s->status = CGYM_SEGMENT_IDLE;
		s->sock = sock;
	} else {
		rc = 1;
	}
	
	return rc;
}

/*
 * returneaza valoarea campului entry al segmentului
 */
cgym_entry_t *cgym_segment_entry(cgym_segment_t *s) {
	cgym_entry_t *rc = NULL;
	
	if (s != NULL)
		rc = s->entry;
	
	return rc;
}

/*
 * returneaza valoarea campului start al segmentului
 */
unsigned long cgym_segment_start(cgym_segment_t *s) {
	unsigned long rc = 0;
	
	if (s != NULL)
		rc = s->start;
	
	return rc;
}

/*
 * returneaza valoarea campului stop al segmentului
 */
unsigned long cgym_segment_stop(cgym_segment_t *s) {
	unsigned long rc = 0;
	
	if (s != NULL)
		rc = s->stop;
	
	return rc;
}

/*
 * returneaza valoarea campului stop al segmentului
 */
char *cgym_segment_buf(cgym_segment_t *s) {
	char *rc = NULL;
	
	if (s != NULL)
		rc = s->buf;
	
	return rc;
}

/*
 * returneaza valoarea campului sock
 */
enum cgym_segment_status cgym_segment_status(cgym_segment_t *s) {
	enum cgym_segment_status rc = CGYM_SEGMENT_NONE;
	
	if (s != NULL) {
		rc = s->status;
	}
	
	return rc;
}

/* modifica valoarea campului status
 * returneaza
 * 
 * 	0 la succes
 * 	1 la eroare (s este NULL)
 */
int cgym_segment_set_status(cgym_segment_t *s, enum cgym_segment_status st) {
	int rc = 0;
	
	if (s != NULL) {
		s->status = st;
	} else {
		rc = 1;
	}
	
	return rc;
}
	
/*
 * elibereaza resursele segmentului
 * 
 * returneaza 0 la succes, nenul la eroare
 */
void cgym_segment_free(cgym_segment_t *s) {
	if (s != NULL) {
		free(s);
	}
}

/*
 * asambleaza segmentele s si le salveaza
 * intr-un fisier (cu numele din e),
 * dupa care verifica MD5-ul.
 * lista s se termina cu elementul NULL
 *
 * returneaza:
 *	0 la succes
 *	1 la eroare
 */
/*
 * asambleaza segmentele s si le scrie in fd
 * lista s se termina cu elementul NULL
 *
 * returneaza:
 *	0 la succes
 *	1 la eroare
 */
int cgym_segment_assemble(cgym_entry_t *e, cgym_segment_t **s){
	FILE *f;
	char *name = cgym_get_file_name(e->file), *md5;
	int k=0;

	printf("cream %s\n", name);
	if ((f = fopen(name,"w+")) == NULL) {
		printf("eroare la crearea fisierului\n");
		return 1;
	}
	while((s[k])!=NULL){
		if ((s[k])->status != CGYM_SEGMENT_DONE){
			printf("segmentul care incepe de la la offset:%ld nu este complet\n",(*s)->start);
			return 1;
		}
		if (!fseek(f,(s[k])->start,SEEK_SET)){
			if (fwrite(s[k]->buf, s[k]->stop - s[k]->start, 1, f) != 1) {
				printf("eroare la scriere!\n");
				return 1;
			}
		}
		else{
			perror("fseek");
			return 1;
		}
		k++;
	}
	fclose(f);
	md5 = compute_md5(name);
	if(strcmp(e->md5,md5)!=0){
		printf("md5 check failed\n");
		return 1;
	}
	printf("md5: %s\n",md5);
	
	free(name);
	free(md5);
	
	return 0;
}

/*
 * verifica daca toate segmentele din lista
 * sunt terminate
 * 
 * returneaza
 * -1 la eroare
 * 0 daca nu
 * 1 daca da
 */
int cgym_segment_done(cgym_segment_t **s) {
	int rc = -1; // eroare daca nu avem nimic
	
	if (s != NULL) {
		while (*s != NULL) {
			if ((*s)->status != CGYM_SEGMENT_DONE)
				break;
			s++;
		}
		
		if (*s != NULL)
			rc = 0; // nu e gata
		else
			rc = 1; // e gata
	}
	
	return rc;
}
