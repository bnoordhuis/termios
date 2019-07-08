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

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*x))

struct flag
{
	char name[16];
	unsigned long flag;
};

char *progname = "termios";

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

struct flag speeds[] =
{
	  V(B0)
	, V(B50)
	, V(B75)
	, V(B110)
	, V(B134)
	, V(B150)
	, V(B200)
	, V(B300)
	, V(B600)
	, V(B1200)
	, V(B1800)
	, V(B2400)
	, V(B4800)
	, V(B9600)
#ifdef B57600
	, V(B57600)
#endif
#ifdef B115200
	, V(B115200)
#endif
	, V(B19200)
#ifdef B230400
	, V(B230400)
#endif
	, V(B38400)
#ifdef B460800
	, V(B460800)
#endif
#ifdef B500000
	, V(B500000)
#endif
#ifdef B57600
	, V(B57600)
#endif
#ifdef B921600
	, V(B921600)
#endif
#ifdef B1000000
	, V(B1000000)
#endif
#ifdef B1152000
	, V(B1152000)
#endif
#ifdef B1500000
	, V(B1500000)
#endif
#ifdef B2000000
	, V(B2000000)
#endif
#ifdef B2500000
	, V(B2500000)
#endif
#ifdef B3000000
	, V(B3000000)
#endif
#ifdef B3500000
	, V(B3500000)
#endif
#ifdef B4000000
	, V(B4000000)
#endif
};
#undef V

void
die(char *fmt, ...)
{
	va_list ap;
	int err;

	err = errno;
	fprintf(stderr, "%s: ", progname);
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, ": %s\n", strerror(err));
	fflush(stderr);
	exit(1);
}

void
printspeed(unsigned long flag, int fst, int speedonly)
{
	struct flag *p;

	for (p = speeds; p < speeds + ARRAY_SIZE(speeds); p++)
		if (flag == p->flag)
		{
			printf("%s%s", &"|"[fst], &p->name[speedonly]);
			return;
		}

	printf("%s%lu", &"|"[fst], flag);
}

void
print(char *name, unsigned long flag, struct flag flags[], unsigned nflags)
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

	if (flags == c_cflag)
		printspeed(flag, fst, /* speedonly */ 0);

	printf("\n");
}

int
main(int argc, char **argv)
{
	struct termios t;
	cc_t *cc;
	char *p, *q;
	int fd;

	progname = argv[0];

	fd = 0;
	if (argc > 1)
	{
		p = q = argv[1];
		fd = (int) strtol(p, &q, 10);

		if (*q != '\0')
			fd = open(p, O_RDONLY);

		if (fd == -1)
			die("open(%s)", p);
	}

	if (tcgetattr(fd, &t))
		die("tcgetattr");

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

	/* Not all platforms support c_ispeed and c_ospeed. */
#ifndef _AIX
	printf("c_ispeed\t"); printspeed(t.c_ispeed, 1, 1); printf("\n");
	printf("c_ospeed\t"); printspeed(t.c_ospeed, 1, 1); printf("\n");
#endif

	return 0;
}
