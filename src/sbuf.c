#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sbuf.h"

static unsigned
next_power(unsigned x)
{
	--x;
	x |= x >> 0x01;
	x |= x >> 0x02;
	x |= x >> 0x04;
	x |= x >> 0x08;
	x |= x >> 0x10;
	return ++x;
}

/* Extend the buffer in sb by at least len bytes.
 * Note len should include the space required for the NUL terminator */
static void
sbuf_extendby(sbuf_t *sb, int len)
{
	char* buf;

	len += sb->NUL;
	if (len <= sb->buflen)
		return;
	if (!sb->buflen)
		sb->buflen = 32;

	sb->buflen = next_power(len);
	printf("--- expand: %d\n", sb->buflen);
	buf = realloc(sb->buf, sb->buflen);
	sb->buf = buf;
}

static void
sbuf_vprintf(sbuf_t *sb, const char *fmt, const va_list ap)
{
	int num_required;
	while ((num_required = vsnprintf(sb->buf + sb->NUL, sb->buflen - sb->NUL, fmt, ap)) >= sb->buflen - sb->NUL)
		sbuf_extendby(sb, num_required + 1);
	sb->NUL += num_required;
}

////////////////////////////////////////////////////////////////////////////////

void
sbuf_init(sbuf_t *sb, size_t reserve)
{
	sb->buf = NULL;
	sb->NUL = 0;
	sb->buflen = 0;

	if (reserve)
		sbuf_extendby(sb, reserve + 1);
}

void
sbuf_cleanup(sbuf_t *sb)
{
	if (sb->buf)
		free(sb->buf);
}

void
sbuf_clear(sbuf_t *sb)
{
	sb->NUL = 0;
	if (sb->buf)
		sb->buf[sb->NUL] = '\0';
}

void
sbuf_cat(sbuf_t *sb, const char *str)
{
	size_t len = strlen(str);

	sbuf_extendby(sb, len + 1);
	memcpy(&sb->buf[sb->NUL], str, len);
	sb->NUL += len;
	sb->buf[sb->NUL] = '\0';
}

void
sbuf_putc(sbuf_t *sb, const char c)
{
	sbuf_extendby(sb, 2);
	sb->buf[sb->NUL++] = c;
	sb->buf[sb->NUL] = '\0';
}

void
sbuf_ncat(sbuf_t *sb, const char *str, size_t len)
{
	sbuf_extendby(sb, len + 1);
	memcpy(&sb->buf[sb->NUL], str, len);
	sb->NUL += len;
	sb->buf[sb->NUL] = '\0';
}

void
sbuf_sprintf(sbuf_t *sb, const char *fmt, ...)
{
	sbuf_clear(sb);

	va_list ap;
	va_start(ap, fmt);
	sbuf_vprintf(sb, fmt, ap);
	va_end(ap);
}

void
sbuf_scatf(sbuf_t *sb, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	sbuf_vprintf(sb, fmt, ap);
	va_end(ap);
}
