#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

/*struct log_t {
	const char *logfile;
};

log_t *newlog(const char *fn)
{
	if (access(fn, W_OK))
		remove(fn);

	log_t *l = malloc(sizeof(log_t));
	l->logfile = fn;
	return l;
}

void log(log_t *l, const char *msg, ...)
{
	va_list args;
	FILE *fp = fopen(l->logfile, "a");

	va_start(args, msg);
	vfprintf(fp, msg, args);
	va_end(args);
	fclose(fp);
}*/

void die(const char *err, ...)
{
	va_list args;

	va_start(args, err);
	vfprintf(stderr, err, args);
	va_end(args);
	exit(EXIT_FAILURE);
}
