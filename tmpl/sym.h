#ifndef _SYM_H_
#define _SYM_H_

void initsym(void);
const char *newsym(const char *);
const char *getsym(const char *);
void setsym(const char *, const char *);
void delsym(const char *);

#endif /* _SYM_H_ */
