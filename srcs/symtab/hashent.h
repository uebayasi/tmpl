/* hashent */

#include "util.h"

struct hashent {
	// XXXLUKEM: a SIMPLEQ might be more appropriate
	TAILQ_ENTRY(hashent) h_next;
	const char *h_names[2];		/* the string */
#define	h_name1	h_names[0]
#define	h_name2	h_names[1]
#define	h_name	h_name1
	unsigned int	h_hv;			/* its hash value */
	const void	*h_data;		/* other values (for name=value) */
};

/*
 * Make a new hash entry, setting its h_next to NULL.
 * If the free list is not empty, use the first entry from there,
 * otherwise allocate a new entry.
 */
static inline struct hashent *
h_new2(const char *name1, const char *name2, unsigned int h, const void *data)
{
	struct hashent *hp;

	hp = ecalloc(1, sizeof(*hp));

	hp->h_name1 = name1;
	hp->h_name2 = name2;
	hp->h_hv = h;
	hp->h_data = data;
	return (hp);
}

static inline struct hashent *
h_new(const char *name, unsigned int h, const void *data)
{
	return h_new2(name, NULL, h, data);
}
