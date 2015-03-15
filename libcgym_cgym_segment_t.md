
```
struct {
	cgym_entry_t *entry;
	unsigned long start;
	unsigned long stop;
} cgym_segment_t;

/*
 * initializeaza segmentul cu entry-ul e, octetul de inceput start (inclusiv)
 * si octetul de sfarsit stop (exclusiv)
 * returneaza segmentul nou la succes, NULL la eroare
 */
cgym_segment_t *cgym_segment_init(cgym_entry_t *e, unsigned long start, unsigned long stop)

/*
 * returneaza valoarea campului entry al segmentului
 */
cgym_entry_t *cgym_segment_entry(cgym_segment_t *s)

/*
 * returneaza valoarea campului start al segmentului
 */
unsigned long cgym_segment_start(cgym_segment_t *s)

/*
 * returneaza valoarea campului stop al segmentului
 */
unsigned long cgym_segment_stop(cgym_segment_t *s)

/*
 * elibereaza resursele segmentului
 * returneaza 0 la succes, nenul la eroare
 * la succes, pune s pe NULL
 */
int cgym_segment_free(cgym_segment_t **s)
```