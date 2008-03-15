#include <stdlib.h>
#include "libcgym.h"
#include "libcgym_priv.h"

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
 * asambleaza segmentele s si le scrie in fd
 * lista s se termina cu elementul NULL
 *
 * returneaza:
 *	0 la succes
 *	1 la eroare
 */
int cgym_segment_assemble(cgym_entry_t *e, char *md5, cgym_segment_t **s);

/*
 * verifica daca toate segmentele din lista
 * sunt terminate
 * 
 * returneaza
 * 0 daca nu
 * 1 daca da
 */
int cgym_segment_done(cgym_segment_t **s);
