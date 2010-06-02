CC = gcc
CFLAGS = -Wall -Werror -pedantic -fPIC --std=gnu99 -I../include -O0 -g -D_GNU_SOURCE
LDFLAGS = -shared -Wl,-soname,libsmallhttp.so.1

SRC = services.c     \
	  conn.c         \
	  hashtable.c    \
	  http.c         \
	  parser.c       \
	  poll_mgmt.c    \
	  response.c     \
	  skipset.c      \
	  smallhttp.c    \
	  urlencode.c    \
	  util.c         \
	  websocket.c    \
	  ibuf.c         \
	  sbuf.c
OBJ = ${SRC:.c=.o}

all: libsmallhttp.so

libsmallhttp.so: ${OBJ}
	@echo CC -o $@
	@${CC} ${LDFLAGS} -o $@.1.0 ${OBJ}
	@ln -f $@.1.0 ../lib/$@
	@ln -f $@.1.0 ../lib/$@.1
	@ln -f $@.1.0 ../lib/$@.1.0

%.o: %.c
	@echo CC $@
	@${CC} -o $@ -c ${CFLAGS} $<

clean:
	@echo cleaning...
	@rm -f libsmallhttp.so.1.0 ${OBJ} ../lib/*

.PHONY: all clean
