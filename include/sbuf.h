#ifndef SBUF_H
#define SBUF_H

#include <stdarg.h>

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
    #define GNUC_PRINTF_CHECK(fmt_idx, arg_idx) __attribute__((format (printf, fmt_idx, arg_idx)))
#else
    #define GNUC_PRINTF_CHECK(fmt_idx, arg_idx)
#endif

typedef struct _sbuf {
    char *buf;
    int NUL;
    int buflen;
} sbuf_t;

#define sbuf_raw(SB) (SB->NUL ? SB->buf : NULL)
#define sbuf_len(SB) (SB->NUL)

/* shorthand to get a char * from an sbuf_t */
#define _S(SB) sbuf_raw(SB)

void sbuf_init(sbuf_t *sb, size_t reserve);
void sbuf_cleanup(sbuf_t *sb);

void sbuf_clear(sbuf_t *sb);
char *sbuf_detach(sbuf_t *sb);
void sbuf_move(sbuf_t *src, sbuf_t *dest);
void sbuf_dup(sbuf_t *src, sbuf_t *dest);

void sbuf_putc(sbuf_t *sb, const char c);

void sbuf_cat(sbuf_t *sb, const char *str);
void sbuf_ncat(sbuf_t *sb, const char *str, size_t len);
void sbuf_bcat(sbuf_t *sb, const sbuf_t *buf);

int sbuf_cmp(sbuf_t *sb, const char *str);
int sbuf_ncmp(sbuf_t *sb, const char *str, size_t len);
int sbuf_bcmp(sbuf_t *sb, const sbuf_t *buf);

void sbuf_sprintf(sbuf_t *sb, const char *fmt, ...) GNUC_PRINTF_CHECK(2,3);
void sbuf_scatf(sbuf_t *sb, const char *fmt, ...) GNUC_PRINTF_CHECK(2,3);

#endif
