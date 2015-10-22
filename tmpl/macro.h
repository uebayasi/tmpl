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

struct macro_scan_ops {
	void (*proc)(const char *);
	void *(*suspend)(void);
	void (*resume)(void *);
};

void initmacro(struct macro_scan_ops *);
int ispushed(void);
void push(int op);
int pop(const char **);
void save(char c);
int end(void);
void define(const char *);
void expand(void);
void template(void);

#endif /* _MACRO_H_ */
