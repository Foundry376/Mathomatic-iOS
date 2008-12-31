# Makefile for compiling, testing, and installing Mathomatic under any UNIX-like OS.
# Currently uses gcc only options in CFLAGS, just remove them for other C compilers.

VERSION		= `cat VERSION`
CFLAGS		+= -O2 -Wuninitialized -Wunused -Wshadow -Wformat -Wparentheses -Wno-char-subscripts # gcc specific flags
#CFLAGS		+= -Wall # Copy cproto.h to proto.h to compile with -Wall with no complaints.
CFLAGS		+= -DUNIX -DVERSION=\"$(VERSION)\" # C compiler flags
LDLIBS		+= -lm # libraries to link

# "make READLINE=1" to include readline support:
CFLAGS		+= $(READLINE:1=-DREADLINE)
LDLIBS		+= $(READLINE:1=-lreadline -lncurses)

# Install directories:
prefix		?= /usr/local
bindir		?= $(prefix)/bin
mandir		?= $(prefix)/share/man
docdir		?= $(prefix)/share/doc

MANHTML		= doc/mathomatic.1.html doc/matho-primes.1.html doc/matho-pascal.1.html doc/matho-sumsq.1.html
AOUT		= mathomatic
M4SCRIPTNAME	= math
M4SCRIPTPATH	= $(bindir)/$(M4SCRIPTNAME)

INCLUDES	= includes.h am.h externs.h complex.h proto.h
OBJECTS		= main.o globals.o am.o solve.o help.o parse.o cmds.o simplify.o \
		  factor.o super.o unfactor.o poly.o diff.o integrate.o \
		  complex.o complex_lib.o list.o gcd.o factor_int.o

MAN1		= mathomatic.1
DOCS		= VERSION AUTHORS COPYING README.txt changes.txt

all: $(AOUT) $(MANHTML) # make these by default

doc: $(MANHTML)

check test: all
	@echo Testing Mathomatic...
	cd tests && time ../$(AOUT) -t all 0<&- >test.out && diff -u all.out test.out
	@rm tests/test.out
	@echo All tests passed.

baseline:
	cd tests && ../$(AOUT) -t all 0<&- >all.out
	@rm -f tests/test.out
	@echo File tests/all.out updated with current test output.

$(OBJECTS): $(INCLUDES) VERSION

$(AOUT): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $(AOUT)

static: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -static $(LDLIBS) -o $(AOUT)

# Here we convert the man pages to HTML docs:
doc/mathomatic.1.html: mathomatic.1
	groff -mandoc -Thtml mathomatic.1 >doc/mathomatic.1.html

doc/matho-primes.1.html: primes/matho-primes.1
	groff -mandoc -Thtml primes/matho-primes.1 >doc/matho-primes.1.html

doc/matho-pascal.1.html: primes/matho-pascal.1
	groff -mandoc -Thtml primes/matho-pascal.1 >doc/matho-pascal.1.html

doc/matho-sumsq.1.html: primes/matho-sumsq.1
	groff -mandoc -Thtml primes/matho-sumsq.1 >doc/matho-sumsq.1.html

# The following creates a shell script allowing easy entry of trig functions.  Only works with GNU m4.
m4install: install
	echo '#!/bin/sh' >$(M4SCRIPTPATH)
	echo '# Shell script to run Mathomatic with the m4 preprocessor.' >>$(M4SCRIPTPATH)
	echo '# Usage: $(M4SCRIPTNAME) [ input_files ]' >>$(M4SCRIPTPATH)
	echo >>$(M4SCRIPTPATH)
	echo 'm4 -eP $(docdir)/mathomatic/m4/functions.m4 $$* - | mathomatic -ru' >>$(M4SCRIPTPATH)
	chmod 0755 $(M4SCRIPTPATH)
	install -m 0755 m4/rmath $(bindir)
	@echo Type \"$(M4SCRIPTNAME)\" to run m4 Mathomatic.

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
	@echo Type \"$(AOUT)\" to run Mathomatic.

uninstall:
	rm -f $(bindir)/$(AOUT)
	cd $(mandir)/man1 && rm -f $(MAN1)
	rm -rf $(docdir)/mathomatic
	rm -f $(prefix)/share/applications/mathomatic.desktop
	rm -f $(prefix)/share/pixmaps/mathomatic.png
	@echo Uninstall completed.

clean:
	rm -f *.o *.a
	rm -f */*.o */*.a */*.pyc
	rm -f tests/test.out

flush: clean
	rm -f $(AOUT)
	rm -f mathomatic_secure
	rm -f $(MANHTML)
