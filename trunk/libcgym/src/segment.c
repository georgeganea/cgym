#include <stdlib.h>
#include "libcgym.h"
#include "libcgym_priv.h"

cgym_segment_t *cgym_segment_init(cgym_entry_t *e,
				unsigned long start, unsigned long stop) {
	cgym_segment_t *rc = NULL;
	
	if (e != NULL) {
		if ((rc = malloc(sizeof(*rc))) != NULL) {
			rc->entry = e;
			rc->start = start;
			rc->stop = stop;
		}
	}
	
	return rc;
}
			
cgym_entry_t *cgym_segment_entry(cgym_segment_t *s) {
	cgym_entry_t *rc = NULL;
	
	if (s != NULL)
		rc = s->entry;
	
	return rc;
}

unsigned long cgym_segment_start(cgym_segment_t *s) {
	unsigned long rc = 0;
	
	if (s != NULL)
		rc = s->start;
	
	return rc;
}

unsigned long cgym_segment_stop(cgym_segment_t *s) {
	unsigned long rc = 0;
	
	if (s != NULL)
		rc = s->stop;
	
	return rc;
}

void cgym_segment_free(cgym_segment_t *s) {
	free(s);
}
