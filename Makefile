CC = gcc
CFLAGS = -Wall -pedantic --std=gnu99 -I. -O2
LDFLAGS =

SRC = staticserver.c mplexsocks.c http/parser.c http/request.c util.c util/hashtable.c
OBJ = ${SRC:.c=.o}

all: staticserver

.c.o:
	@echo CC $@
	@${CC} -o $@ -c ${CFLAGS} $<

staticserver: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning...
	@rm -f staticserver ${OBJ}
