#ifndef LIBS_IO
#define LIBS_IO

#include <stddef.h>

typedef struct io IO;

struct io *io_new_fd(int fd);
struct io *io_ref(struct io *io);
void io_close(struct io *io);

int io_get_fd(struct io *io);

int io_eof(struct io *io);
size_t io_read(struct io *io, char *buf, size_t len);
char io_getc(struct io *io);

size_t io_write(struct io *io, char *buf, size_t len);

#include "watch.h"
#include "buf.h"

#endif
