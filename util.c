#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

void die(const char *err, ...)
{
	va_list args;

	fprintf(stderr, "error %d: %s\n", errno, strerror(errno));
	va_start(args, err);
	vfprintf(stderr, err, args);
	va_end(args);
	exit(EXIT_FAILURE);
}
