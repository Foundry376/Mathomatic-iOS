# Makefile for compiling, testing, and installing Mathomatic under any UNIX-like OS.
# Currently uses gcc only options in CFLAGS, just remove them for other C compilers.

VERSION		= `cat VERSION`
CFLAGS		+= -O2 -Wuninitialized -Wunused -Wshadow -Wformat -Wparentheses # gcc specific flags
CFLAGS		+= -DUNIX -DVERSION=\"$(VERSION)\" # C compiler flags
LIBS		+= -lm # libraries to link

# "make READLINE=1" to include readline support:
CFLAGS		+= $(READLINE:1=-DREADLINE)
LIBS		+= $(READLINE:1=-lreadline -lncurses)

# Install directories:
prefix		?= /usr/local
bindir		?= $(prefix)/bin
mandir		?= $(prefix)/share/man
docdir		?= $(prefix)/share/doc

MANHTML		= doc/mathomatic.1.html doc/matho-primes.1.html doc/matho-pascal.1.html doc/matho-sumsq.1.html
AOUT		= mathomatic
M4SCRIPT	= $(bindir)/math

OBJECTS		= main.o globals.o am.o solve.o help.o parse.o cmds.o simplify.o \
		  factor.o super.o unfactor.o poly.o diff.o integrate.o \
		  complex.o complex_lib.o list.o gcd.o factor_int.o

MAN1		= mathomatic.1
DOCS		= VERSION AUTHORS COPYING README.txt changes.txt

all: $(AOUT) $(MANHTML) # make these by default
	@echo Make completed.

doc: $(MANHTML)

check test:
	@echo Testing Mathomatic...
	cd tests && time ../$(AOUT) -t all 0<&- >test.out && diff -u all.out test.out
	@rm tests/test.out
	@echo All tests passed.

baseline:
	cd tests && ../$(AOUT) -t all 0<&- >all.out
	@rm -f tests/test.out
	@echo File tests/all.out updated with current test output.

$(OBJECTS): includes.h am.h externs.h complex.h proto.h VERSION

$(AOUT): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $(AOUT)

# Here we convert the man pages to HTML docs:
doc/mathomatic.1.html: mathomatic.1
	@rman --version # display version number and test for existence
	rman -f HTML mathomatic.1 >doc/mathomatic.1.html

doc/matho-primes.1.html: primes/matho-primes.1
	@rman --version
	rman -f HTML primes/matho-primes.1 >doc/matho-primes.1.html

doc/matho-pascal.1.html: primes/matho-pascal.1
	@rman --version
	rman -f HTML primes/matho-pascal.1 >doc/matho-pascal.1.html

doc/matho-sumsq.1.html: primes/matho-sumsq.1
	@rman --version
	rman -f HTML primes/matho-sumsq.1 >doc/matho-sumsq.1.html

# The following creates a shell script allowing easy entry of trig functions.  Only works with GNU m4.
m4install: install
	echo '#!/bin/sh' >$(M4SCRIPT)
	echo '# Shell script to run Mathomatic with the m4 preprocessor.' >>$(M4SCRIPT)
	echo '# Usage: math [ input_files ]' >>$(M4SCRIPT)
	echo >>$(M4SCRIPT)
	echo 'm4 -eP $(docdir)/mathomatic/m4/functions.m4 $$* - | mathomatic -ru' >>$(M4SCRIPT)
	chmod 0755 $(M4SCRIPT)
	@echo Type \"math\" to run m4 Mathomatic.

install:
	install -d $(bindir)
	install -d $(mandir)/man1
	install -d $(docdir)/mathomatic
	install -d $(docdir)/mathomatic/html
	install -d $(docdir)/mathomatic/m4
	install -d $(docdir)/mathomatic/tests
	install -d $(docdir)/mathomatic/factorial
	install -d $(prefix)/share/applications
	install -d $(prefix)/share/pixmaps
	install -m 0755 $(AOUT) $(bindir)
	install -m 0644 $(MAN1) $(mandir)/man1
	install -m 0644 $(DOCS) $(docdir)/mathomatic
	install -m 0644 doc/* $(docdir)/mathomatic/html
	install -m 0644 m4/* $(docdir)/mathomatic/m4
	install -m 0644 tests/* $(docdir)/mathomatic/tests
	install -m 0644 factorial/* $(docdir)/mathomatic/factorial
	install -m 0644 icons/mathomatic.desktop $(prefix)/share/applications
	install -m 0644 icons/mathomatic.png $(prefix)/share/pixmaps
	@echo Install completed.
	@echo Documentation is installed in $(docdir)/mathomatic/html

uninstall:
	rm -f $(bindir)/$(AOUT) $(M4SCRIPT)
	cd $(mandir)/man1 && rm -f $(MAN1)
	rm -rf $(docdir)/mathomatic
	rm -f $(prefix)/share/applications/mathomatic.desktop
	rm -f $(prefix)/share/pixmaps/mathomatic.png
	@echo Uninstall completed.

clean:
	rm -f *.o *.a
	rm -f lib/*.o lib/*.a
	rm -f tests/test.out

flush: clean
	rm -f $(AOUT)
	rm -f mathomatic_secure
