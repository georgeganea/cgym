
```
struct {
	int type; /* NONE, FILE, DIRECTORY */
	unsigned long size; /* 0 for directory */
	char *md5; /* NULL for directory */
	char *file;
} cgym_entry_t;

/*
 * aloca resurse pentru un entry si salveaza numele acestuia
 *
 * returneaza entry-ul la succes, NULL la eroare
 */
cgym_entry_t *cgym_entry_init(char *file)

/*
 * returneaza tipul entry-ului { NONE, FILE, DIRECTORY }
 */
int cgym_entry_type(cgym_entry_t *e)

/*
 * returneaza campul size al entry-ului
 */
unsigned long cgym_entry_size(cgym_entry_t *e)

/*
 * returneaza campul MD5 al entry-ului
 */
char *cgym_entry_md5(cgym_entry_t *e)

/*
 * returneaza campul file al entry-ului
 */
char *cgym_entry_file(cgym_entry_t *e)

/*
 * elibereaza resursele entry-ului
 * returneaza 0 la succes, nenul la eroare
 * la succes, pune e pe NULL
 */
int cgym_entry_free(cgym_entry_t **e)
```