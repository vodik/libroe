#include "urlencode.h"

#include <stdlib.h>
#include <string.h>

static inline char from_hex(char c)
{
	return isdigit(c) ? c - '0' : tolower(c) - 'a' + 10;
}

static inline char to_hex(char code)
{
	static char hex[] = "0123456789abcdef";
	return hex[code & 15];
}

size_t url_encode(char *dest, const char *src)
{
	size_t written = 0;
	while (*src) {
		if (isalnum(*src) || *src == '-' || *src == '_' || *src == '.' || *src == '~')
			*dest++ = *src;
		else if (*src == ' ')
			*dest++ = '+';
		else {
			*dest++ = '%';
			*dest++ = to_hex(*src >> 4);
			*dest++ = to_hex(*src & 15);
			written += 2;
		}
		++src;
		++written;
	}
	*dest = '\0';
	return written;
}

size_t url_decode(char *dest, const char *src)
{
	size_t written = 0;
	while (*src) {
		if (*src == '%') {
			if (src[1] && src[2]) {
				*dest = from_hex(src[1]) << 4;
				*dest++ |= from_hex(src[2]);
				src += 2;
			}
		} else if (*src == '+')
			*dest++ = ' ';
		else
			*dest++ = *src;
		++src;
		++written;
	}
	*dest = '\0';
	return written;
}
