CC = gcc
CFLAGS = -Wall -pedantic --std=gnu99 -I../include -O0 -g -D_GNU_SOURCE
LDFLAGS =

SRC = deflate.c      \
	  hashtable.c    \
	  http.c         \
	  main.c         \
	  parser.c       \
	  poll_mgmt.c    \
	  response.c     \
	  skipset.c      \
	  smallhttp.c    \
	  uri.c          \
	  urlencode.c    \
	  util.c         \
	  websocket.c    \
	  ibuf.c         \
	  sbuf.c
OBJ = ${SRC:.c=.o}

all: __temp

__temp: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

%.o: %.c
	@echo CC $@
	@${CC} -o $@ -c ${CFLAGS} $<

clean:
	@echo cleaning...
	@rm -f test ${OBJ}

.PHONY: all clean
