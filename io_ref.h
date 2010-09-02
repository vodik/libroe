#ifndef LIBS_IO_REF
#define LIBS_IO_REF

#include "buf.h"

struct io {
	int fd;
	unsigned refs;

	void (*iofunc)(struct io *, int, void *);
	void *arg;
	struct buf *buf;
};

#endif
