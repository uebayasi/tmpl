/* dbg */

#ifndef DEBUG
#define	DBG(...)	do {} while (0)
#define	DBGINDENT()	do {} while (0)
#define	DUMPCHAR(l,c,n)	do {} while (0)
#define	DUMPBUF()	do {} while (0)
#else
#define	DBGINDENT()	do { \
	int d = depth(); \
	while (d++ < MACRO_DEPTH - 1) fputc('\t', stderr); \
} while (0)
#define	DBG(...)	do { \
	DBGINDENT(); \
	fprintf(stderr, __VA_ARGS__); \
} while (0)
#define	PC(c)		((vc(c) == 0) ? '?' : vc(c))
#define	DUMPCHAR(l, c, n)	do { \
	if ((c) < 0x20) \
		fprintf(stderr, "%c%c%c%c%s", (l), '\\', PC(c), (l) + 2, (n)); \
	else \
		fprintf(stderr, "%c%c%c%c%s", (l), ' ', c, (l) + 2, (n)); \
} while (0)
#define	DUMPBUF()	do { \
	char *str; \
	fprintf(stderr, "===|"); \
	for (str = sb->head; str != sb->tail; str++) { \
		char c = *str; \
		DUMPCHAR('[', c, ""); \
	} \
	fprintf(stderr, "|===\n"); \
} while (0)
#endif
