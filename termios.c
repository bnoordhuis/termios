/* Copyright (c) 2019, Ben Noordhuis <info@bnoordhuis.nl>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*x))

struct flag
{
	char name[16];
	tcflag_t flag;
};

/* Keep the entries sorted alphabetically. */
#define V(name) { #name, name }
struct flag c_iflag[] =
{
	  V(BRKINT)
	, V(ICRNL)
	, V(IGNBRK)
	, V(IGNCR)
	, V(IGNPAR)
#ifdef IMAXBEL
	, V(IMAXBEL)
#endif
	, V(INLCR)
	, V(ISTRIP)
#ifdef IUTF8
	, V(IUTF8)
#endif
	, V(IXANY)
	, V(IXOFF)
	, V(IXON)
	, V(PARMRK)
};

struct flag c_oflag[] =
{
	  V(BS1)
	, V(NL1)
	, V(ONLCR)
#ifdef ONOEOT
	, V(ONOEOT)
#endif
	, V(OPOST)
#ifdef OXTABS
	, V(OXTABS)
#endif
};

struct flag c_cflag[] =
{
	  V(CLOCAL)
	, V(CREAD)
	, V(CSIZE)
	, V(CSTOPB)
	, V(HUPCL)
};

struct flag c_lflag[] =
{
	  V(ECHO)
	, V(ECHOCTL)
	, V(ECHOE)
	, V(ECHOK)
	, V(ECHOKE)
	, V(ECHONL)
	, V(ECHOPRT)
	, V(FLUSHO)
	, V(ICANON)
	, V(IEXTEN)
	, V(ISIG)
	, V(NOFLSH)
	, V(PENDIN)
	, V(TOSTOP)
#ifdef XCASE
	, V(XCASE)
#endif
};
#undef V

void
print(char *name, tcflag_t flag, struct flag flags[], unsigned nflags)
{
	struct flag *p;
	int fst;

	printf("%s\t\t", name);

	fst = 1;
	for (p = flags; p < flags + nflags; p++)
		if (flag & p->flag)
		{
			printf("%s%s", &"|"[fst], p->name);
			flag -= p->flag;
			fst = 0;
		}

	if (flag)
			printf("%s%#lx", &"|"[fst], flag);

	printf("\n");
}

int
main(int argc, char **argv)
{
	struct termios t;
	cc_t *cc;
	char *p, *q;
	int fd;

	fd = 0;
	if (argc > 1)
	{
		p = q = argv[1];
		fd = (int) strtol(p, &q, 10);

		if (*q != '\0')
			fd = open(p, O_RDONLY);

		if (fd == -1)
			err(1, "open(%s)", p);
	}

	if (tcgetattr(fd, &t))
		err(1, "tcgetattr");

#define V(name) print(#name, t.name, name, ARRAY_SIZE(name))
	V(c_iflag);
	V(c_oflag);
	V(c_cflag);
	V(c_lflag);
#undef V

	printf("c_cc\t\t");
	for (cc = t.c_cc; cc < t.c_cc + ARRAY_SIZE(t.c_cc); cc++)
		printf("%s%u", &","[cc == t.c_cc], *cc);
	printf("\n");

	printf("c_ispeed\t%lu\n", t.c_ispeed);
	printf("c_ospeed\t%lu\n", t.c_ospeed);

	return 0;
}
