#ifndef IO_BUF
#define IO_BUF

#include <stddef.h>

typedef size_t (*buf_fill)(char *buf, size_t len, void *arg);
typedef size_t (*buf_push)(const char *buf, size_t len, void *arg);

struct buf;
struct obuf;
struct ibuf;

struct ibuf *ibuf_new(size_t size, buf_fill fill, void *arg);
void ibuf_free(struct ibuf *buf);
size_t ibuf_read(struct ibuf *buf, char *dest, size_t len);

struct obuf *obuf_new(size_t size, buf_push push, void *arg);
void obuf_free(struct obuf *buf);
size_t obuf_write(struct obuf *buf, const char *src, size_t len);
void obuf_flush(struct obuf *buf);

struct buf *buf_new(size_t size, buf_fill fill, buf_push push, void *arg);
void buf_free(struct buf *buf);
size_t buf_read(struct buf *buf, char *dest, size_t len);
size_t buf_write(struct buf *buf, const char *src, size_t len);
void buf_flush(struct buf *buf);

#endif
