DIRS = src test

all: ${DIRS}

${DIRS}: force
	@${MAKE} -C $@ all

force:
	@true

clean:
	@echo cleaning...

.PHONY: all clean
