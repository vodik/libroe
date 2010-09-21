CC = gcc
CFLAGS = -Wall -Werror -pedantic -std=gnu99 -O3
LDFLAGS = -lm

EXE = libroe_test
SRC = test.c conn.c services.c io.c watch.c http.c websockets.c network.c util.c obuf.c ibuf.c buf.c parser.c string.c hashtable.c
OBJ = ${SRC:.c=.o}

all: ${EXE}

${EXE}: ${OBJ}
	@echo CC -o $@
	@${CC} ${LDFLAGS} -o $@ ${OBJ}

%.o: %.c
	@echo CC $@
	@${CC} -o $@ -c $< ${CFLAGS}

clean:
	@echo cleaning...
	@rm ${EXE} ${OBJ}

.PHONY: all clean
