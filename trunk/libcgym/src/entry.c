#include "libcgym.h"

struct cgym_entry_t_ {
        enum cgym_entry_type type; /* NONE, FILE, DIRECTORY */
        unsigned long size; /* 0 for directory */
        char *md5; /* NULL for directory */
        char *file;
};

cgym_entry_t *cgym_entry_init(char *file);
int cgym_entry_type(cgym_entry_t *e);
unsigned long cgym_entry_size(cgym_entry_t *e);
char *cgym_entry_md5(cgym_entry_t *e);
char *cgym_entry_file(cgym_entry_t *e);
int cgym_entry_free(cgym_entry_t **e);
