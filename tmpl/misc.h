#ifndef _MISC_H_
#define _MISC_H_

#ifndef nitems
#define	nitems(_a)	(sizeof((_a)) / sizeof((_a)[0]))
#endif

static inline char *overwrite(const char *cs) {
	union {
		const char *cs;
		char *s;
	} u = {
		.cs = cs
	};
	return u.s;
}

#endif /* _MISC_H_ */
