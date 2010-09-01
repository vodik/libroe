#ifndef LIBS_IO
#define LIBS_IO

#include <stddef.h>

struct io;

struct io *io_new_fd(int fd);
void io_ref(struct io *io);
void io_close(struct io *io);

int io_get_fd(struct io *io);

size_t io_read(struct io *io, char *buf, size_t len);
size_t io_write(struct io *io, char *buf, size_t len);

#include "watch.h"

#endif
