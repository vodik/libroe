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

	if (len > sb->buflen)
		sb->buflen = next_power(len);

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

static void
sbuf_init(sbuf_t *sb)
{
	sb->buf = NULL;
	sb->NUL = 0;
	sb->buflen = 0;
}

////////////////////////////////////////////////////////////////////////////////

sbuf_t *
sbuf_new(size_t reserve)
{
	sbuf_t *sb = malloc(sizeof(*sb));
	if (!sb)
		return NULL;

	sbuf_init(sb);
	return sb;
}

void
sbuf_free(sbuf_t *sb)
{
	if (sb->buf)
		free(sb->buf);
	free(sb);
}

void
sbuf_clear(sbuf_t *sb)
{
	sb->NUL = 0;
}

char *
sbuf_detach(sbuf_t *sb)
{
	char* buf = sb->buf;
	sbuf_init(sb);
	return buf;
}

void
sbuf_move(sbuf_t *src, sbuf_t *dest)
{
	if (dest->buf)
		free(dest->buf);
	dest->buf = src->buf;
	dest->NUL = src->NUL;
	dest->buflen = src->buflen;
	sbuf_init(src);
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
	sbuf_extendby(sb, 1);
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
	sbuf_init(sb);

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
