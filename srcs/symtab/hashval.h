/* hashval */

static inline unsigned int		hash(unsigned int, const char *);
static inline unsigned int		hash2(unsigned int, const char *, const char *);

static inline unsigned int
hv(unsigned int h, char c)
{
	return (h << 5) + h + (unsigned char)c;
}

/*
 * Hash a string.
 */
static inline unsigned int
hash(unsigned int h, const char *str)
{

	while (str && *str)
		h = hv(h, *str++);
	return (h);
}

#define	HASH2DELIM	' '

static inline unsigned int
hash2(unsigned int h, const char *str1, const char *str2)
{

	h = hash(h, str1);
	h = hv(h, HASH2DELIM);
	h = hash(h, str2);
	return (h);
}
