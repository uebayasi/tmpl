/* intern */

#include <sys/param.h>
#include <sys/queue.h>
#include <stdio.h>
#if 0
#include <util.h>
#include "defs.h"
#endif

#include "util.h"
#include "hashval.h"
#include "hashent.h"
#include "hashtab.h"

#define	DBG(...)	//fprintf(stderr, __VA_ARGS__)

struct hashent *
ht_add2(struct hashtab *ht, const char *name1, const char *name2, void *data,
    unsigned int hv, struct hashenthead *hpp);

void *
ht_cmp2(struct hashtab *ht, const char *name1, const char *name2,
    unsigned int *rh, struct hashenthead **rhpp);

/*
 * Generate a single unique copy of the given string.  We expect this
 * function to be used frequently, so it should be fast.
 */
static const char *
ht_intern(struct hashtab *ht, const char *name)
{
	DBG("<%s>", __func__);
	struct hashent *hp;
	struct hashenthead *hpp;
	unsigned int hv;
	char *sym;

	hp = ht_cmp2(ht, name, NULL, &hv, &hpp);
	if (hp == NULL) {
		sym = estrdup(name);
		hp = ht_add2(ht, sym, NULL, sym, hv, hpp);
	}
	return (hp->h_data);
}

static struct hashtab strings;

void
initintern(void)
{

	ht_init(&strings, 128);
}

const char *
intern(const char *name)
{
	return ht_intern(&strings, name);
}
