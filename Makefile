DIRS = src

all: ${DIRS}

src: force
	@echo building src...
	@cd $@; ${MAKE} ${MFLAGS}

tests: force
	@echo building tests...
	@cd $@; ${MAKE} ${MFLAGS}

force:
	true

clean:
	@echo cleaning...
	@rm -f test ${OBJ}

.PHONY: all clean
