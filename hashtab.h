/* hashtab */

/*
 * Interned strings are kept in a hash table.  By making each string
 * unique, the program can compare strings by comparing pointers.
 */
struct hashtab {
	size_t	ht_size;		/* size (power of 2) */
	size_t	ht_mask;		/* == ht_size - 1 */
	size_t	ht_used;		/* number of entries used */
	size_t	ht_lim;			/* when to expand */
	TAILQ_HEAD(hashenthead, hashent) *ht_tab;
};

void			ht_init(struct hashtab *, size_t);
void			ht_expand(struct hashtab *);

struct hashtab *	ht_new(void);
void			ht_free(struct hashtab *ht);

int ht_insrep2(struct hashtab *ht, const char *name1, const char *name2, const void *data, int replace);
int ht_insrep(struct hashtab *ht, const char *nam, const void *data, int replace);
int ht_remove2(struct hashtab *ht, const char *name1, const char *name2);
int ht_remove(struct hashtab *ht, const char *name);
const void *ht_lookup2(struct hashtab *ht, const char *name1, const char *name2);
const void *ht_lookup(struct hashtab *ht, const char *nam);
