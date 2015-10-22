#include <stdio.h>

int
main(int argc, char *argv[])
{
	int c;

	for (;;) {
		c = getchar();
		switch (c) {
		case EOF:
			exit(0);
			/* NOTREACHED */
		case '~':
			c = getchar();
			switch (c) {
			case EOF:
				exit(0);
				/* NOTREACHED */
			case '~':
				putchar('~');
				break;
			case '{':
			case '}':
			}
			break;
		default;
			putchar(c);
			break;
		}
	}
	return 0;
}
