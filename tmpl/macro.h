/* macro */

#ifndef _MACRO_H_
#define _MACRO_H_

#include "macro_config.h"

struct strbuf {
	char *head, *tail;
};

struct frame {
	struct strbuf strbuf;
	int op;
};

struct stack {
	struct frame *frames;
	int depth;
};

struct macro_ops {
	void (*proc)(const char *);
	void *(*suspend)(void);
	void (*resume)(void *);
};

void initmacro(struct macro_ops *);
int ispushed(void);
void push(int op);
int pop(const char **);
void save(char c);
int end(void);
int define(const char *);
int expand(void);
int repeat(void);
int template(void);

#endif /* _MACRO_H_ */
