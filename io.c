#include "io.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "io_ref.h"
#include "watch.h"
#include "buf.h"

static size_t
io_buf_read(char *buf, size_t len, void *arg)
{
	struct io *io = arg;
	ssize_t ret = read(io->fd, buf, len);
	if (ret < 0) {
		perror("read");
		exit(EXIT_FAILURE); /* TODO error handling */
	}
	return (size_t)ret;
}

static size_t
io_buf_write(const char *buf, size_t len, void *arg)
{
	struct io *io = arg;
	ssize_t ret = write(io->fd, buf, len);
	if (ret < 0) {
		perror("read");
		exit(EXIT_FAILURE); /* TODO error handling */
	}
	return (size_t)ret;
}

struct io *
io_new_fd(int fd)
{
	struct io *io = malloc(sizeof(struct io));
	io->fd = fd;
	io->refs = 1;

	io->iofunc = NULL;
	io->arg = NULL;
	io->buf = buf_new(32, io_buf_read, io_buf_write, io);

	return io;
}

struct io *
io_ref(struct io *io)
{
	++io->refs;
	return io;
}

void
io_close(struct io *io)
{
	if (--io->refs == 0) {
		close(io->fd);
		buf_free(io->buf);
		free(io);
	}
}

int
io_get_fd(struct io *io)
{
	return io->fd;
}

int
io_eof(struct io *io)
{
	return 0;
}

size_t
io_read(struct io *io, char *buf, size_t len)
{
	return buf_read(io->buf, buf, len);
}

char
io_getc(struct io *io)
{
	char c[2];
	buf_read(io->buf, c, 2);
	return c[0];
}

size_t
io_write(struct io *io, char *buf, size_t len)
{
	size_t ret = buf_write(io->buf, buf, len);
	buf_flush(io->buf);
	return ret;
}
