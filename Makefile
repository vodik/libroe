CC = gcc
CFLAGS = -Wall -pedantic --std=gnu99 -I./include -O0 -g -D_GNU_SOURCE
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
      test_http.c    \
      uri.c          \
      urlencode.c    \
      util.c         \
      websocks.c
      #sbuf.c         
OBJ = ${SRC:.c=.o}

all: test

%.o: %.c
	@echo CC $@
	@${CC} -o $@ -c ${CFLAGS} $<

test: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning...
	@rm -f test ${OBJ}
