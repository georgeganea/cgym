#include "libcgym.h"
#include "libcgym_priv.h"

cgym_segment_t *cgym_segment_init(cgym_entry_t *e, unsigned long start, unsigned long stop);
cgym_entry_t *cgym_segment_entry(cgym_segment_t *s);
unsigned long cgym_segment_start(cgym_segment_t *s);
unsigned long cgym_segment_stop(cgym_segment_t *s);
void cgym_segment_free(cgym_segment_t *s);
