/*	$NetBSD$	*/

/*
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This software was developed by the Computer Systems Engineering group
 * at Lawrence Berkeley Laboratory under DARPA contract BG 91-66 and
 * contributed to Berkeley.
 *
 * All advertising materials mentioning features or use of this software
 * must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Lawrence Berkeley Laboratories.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	from: @(#)hash.c	8.1 (Berkeley) 6/6/93
 */

#if HAVE_NBTOOL_CONFIG_H
#include "nbtool_config.h"
#endif

#include <sys/cdefs.h>
//__RCSID("$NetBSD$");

#include <sys/param.h>
#include <sys/queue.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if 0
#include <util.h>
#include "defs.h"
#endif

#include "util.h"
#include "hashval.h"
#include "hashent.h"
#include "hashtab.h"

#define	DBG(...)	//fprintf(stderr, __VA_ARGS__)

void
ht_del(struct hashtab *ht, struct hashenthead *hpp, struct hashent *hp);

/*
 * HASHFRACTION controls ht_lim, which in turn controls the average chain
 * length.  We allow a few entries, on average, as comparing them is usually
 * cheap (the h_hv values prevent a strcmp).
 */
#define	HASHFRACTION(sz) ((sz) * 3 / 2)

/*
 * Initialize a new hash table.  The size must be a power of 2.
 */
void
ht_init(struct hashtab *ht, size_t sz)
{
	unsigned int n;

	ht->ht_tab = emalloc(sz * sizeof (ht->ht_tab[0]));
	ht->ht_size = sz;
	ht->ht_mask = sz - 1;
	for (n = 0; n < sz; n++)
		TAILQ_INIT(&ht->ht_tab[n]);
	ht->ht_used = 0;
	ht->ht_lim = HASHFRACTION(sz);
}

/*
 * Expand an existing hash table.
 */
void
ht_expand(struct hashtab *ht)
{
	struct hashenthead *oldh;
	struct hashenthead *newhpp, *oldhpp;
	struct hashent *hp;
	size_t i, size, used;
	unsigned int hv;

	oldh = ht->ht_tab;
	size = ht->ht_size;
	used = ht->ht_used;
	ht_init(ht, ht->ht_size * 2);
	ht->ht_used = used;

	for (i = 0; i < size; i++) {
		oldhpp = &oldh[i];
		while ((hp = TAILQ_FIRST(oldhpp)) != NULL) {
			TAILQ_REMOVE(oldhpp, hp, h_next);
				// XXXLUKEM: really should be TAILQ_INSERT_TAIL
			hv = hp->h_hv;
			newhpp = &ht->ht_tab[hv & ht->ht_mask];
			TAILQ_INSERT_HEAD(newhpp, hp, h_next);
		}
	}
	free(oldh);
}

/*
 * hash table
 */

struct hashtab *
ht_new(void)
{
	struct hashtab *ht;

	ht = ecalloc(1, sizeof *ht);
	ht_init(ht, 8);
	return (ht);
}

void
ht_free(struct hashtab *ht)
{
	size_t i;
	struct hashent *hp;
	struct hashenthead *hpp;

	for (i = 0; i < ht->ht_size; i++) {
		hpp = &ht->ht_tab[i];
		while ((hp = TAILQ_FIRST(hpp)) != NULL) {
			ht_del(ht, hpp, hp);
		}
	}

	assert(ht->ht_used == 0);
	free(ht->ht_tab);
	free(ht);
}

static void *
ht_find2(struct hashtab *ht, const char *name1, const char *name2,
    unsigned int *rhv, struct hashenthead **rhpp)
{
	DBG("<%s>", __func__);
	struct hashent *hp;
	struct hashenthead *hpp;
	unsigned int hv;

	hv = hash2(0, name1, name2);
	hpp = &ht->ht_tab[hv & ht->ht_mask];
	TAILQ_FOREACH(hp, hpp, h_next) {
		if (hp->h_hv != hv)
			continue;
		if (hp->h_name1 == name1 && hp->h_name2 == name2)
			break;
	}
	if (rhv != NULL)
		*rhv = hv;
	if (rhpp != NULL)
		*rhpp = hpp;
	DBG("<%s done>", __func__);
	return (hp);
}

void *
ht_cmp2(struct hashtab *ht, const char *name1, const char *name2,
    unsigned int *rhv, struct hashenthead **rhpp);

void *
ht_cmp2(struct hashtab *ht, const char *name1, const char *name2,
    unsigned int *rhv, struct hashenthead **rhpp)
{
	DBG("<%s>", __func__);
	struct hashent *hp;
	struct hashenthead *hpp;
	unsigned int hv;

	hv = hash2(0, name1, name2);
	hpp = &ht->ht_tab[hv & ht->ht_mask];
	TAILQ_FOREACH(hp, hpp, h_next) {
		if (hp->h_hv != hv)
			continue;
		if (strcmp(hp->h_name1, name1) != 0)
			continue;
		if (hp->h_name2 != NULL && strcmp(hp->h_name2, name2) != 0)
			continue;
		break;
	}
	if (rhv != NULL)
		*rhv = hv;
	if (rhpp != NULL)
		*rhpp = hpp;
	return (hp);
}

struct hashent *
ht_add2(struct hashtab *ht, const char *name1, const char *name2, const void *data,
    unsigned int hv, struct hashenthead *hpp)
{
	struct hashent *hp; 

	hp = h_new2(name1, name2, hv, data);
	TAILQ_INSERT_TAIL(hpp, hp, h_next);
	if (++ht->ht_used > ht->ht_lim)
		ht_expand(ht);
	return (hp);
}

void
ht_del(struct hashtab *ht, struct hashenthead *hpp, struct hashent *hp)
{

	TAILQ_REMOVE(hpp, hp, h_next);
	free(hp);
	ht->ht_used--;
}

/*
 * Insert and/or replace.
 */
int
ht_insrep2(struct hashtab *ht, const char *name1, const char *name2, const void *data,
    int replace)
{
	struct hashent *hp;
	struct hashenthead *hpp;
	unsigned int hv;

	hp = ht_find2(ht, name1, name2, &hv, &hpp);
	if (hp != NULL) {
		if (replace)
			hp->h_data = data;
		return (1);
	}

	(void)ht_add2(ht, name1, name2, data, hv, hpp);
	return (0);
}

int
ht_insrep(struct hashtab *ht, const char *nam, const void *data, int replace)
{
	return ht_insrep2(ht, nam, NULL, data, replace);
}

/*
 * Remove.
 */
int
ht_remove2(struct hashtab *ht, const char *name1, const char *name2)
{
	struct hashent *hp;
	struct hashenthead *hpp;
	unsigned int hv;

	hp = ht_find2(ht, name1, name2, &hv, &hpp);
	if (hp != NULL) {
		ht_del(ht, hpp, hp);
		return (0);
	}
	return (1);
}

int
ht_remove(struct hashtab *ht, const char *name)
{
	return ht_remove2(ht, name, NULL);
}

const void *
ht_lookup2(struct hashtab *ht, const char *name1, const char *name2)
{
	DBG("<%s>", __func__);
	struct hashent *hp;

	hp = ht_find2(ht, name1, name2, NULL, NULL);
	if (hp != NULL)
		return (hp->h_data);
	return (NULL);
}

const void *
ht_lookup(struct hashtab *ht, const char *nam)
{
	return ht_lookup2(ht, nam, NULL);
}
