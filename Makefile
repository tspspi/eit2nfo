include Makefile.SUPPORTED

ifeq (,$(findstring $(OS),$(SUPPORTEDPLATFORMS)))

all:

	@echo The OS environment variable is currently set to [$(OS)]
	@echo Please set the OS environment variable to one of the following:
	@echo $(SUPPORTEDPLATFORMS)

.PHONY: all

else

include Makefile.$(OS)

OPTIONS=
SRCFILES=src/eit2nfo.c \
	src/eitfile.c

HFILES=include/eitfile.h

OBJFILES=tmp/eit2nfo$(OBJSUFFIX) \
	tmp/eitfile$(OBJSUFFIX)

all: eit2nfo

eit2nfo: $(OBJFILES)

	$(CC) -o bin/eit2nfo$(EXESUFFIX) $(OBJFILES)

tmp/%$(OBJSUFFIX): src/%.c $(LIBHFILES)

	$(CCLIB) $(OPTIONS) -c -o $@ $<

endif
