#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <math.h>

void
die(const char *err, ...)
{
	va_list args;

	fprintf(stderr, "error %d: %s\n", errno, strerror(errno));
	va_start(args, err);
	vfprintf(stderr, err, args);
	va_end(args);
	exit(EXIT_FAILURE);
}

int
itoa(int n, char s[], size_t len, int radix)
{
	int i, t, sign;
	int digits;

	if (radix > 36)
		return 0;

	digits = (int)(log(n) / log(radix)) + 1;
	if (digits > len)
		return 0;
	i = digits - 1;

	if ((sign = n) < 0)
		n = -n;

	do {
		t = n % radix;
		s[i--] = t + (t > 9 ? 'a' - 10 : '0');
	} while (n /= radix);

	if (sign < 0)
		s[i] = '-';

	s[digits] = '\0';
	return digits;
}
