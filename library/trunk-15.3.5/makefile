# Makefile for compiling, testing, and installing Mathomatic under any UNIX-like system.
# Requires the GNU make utility.
# Remove the -DUNIX define in CFLAGS when not using Mathomatic for desktop UNIX/Linux.
# This makefile does not compile and install the Prime Number Tools in the "primes" directory.
# This makefile does create and install most of the Mathomatic documentation.

SHELL		= /bin/sh # from "http://www.gnu.org/prep/standards/", do not change
CC		?= gcc # C compiler to use
INSTALL		?= install # installer to use
INSTALL_PROGRAM	?= $(INSTALL) # command to install executable program files
INSTALL_DATA	?= $(INSTALL) -m 0644 # command to install data files

DEVROOT=/Developer/Platforms/iPhoneSimulator.platform/Developer
SDKROOT=$(DEVROOT)/SDKs/iPhoneSimulator4.1.sdk

CPPFLAGS=-I$(SDKROOT)/usr/lib/gcc/i686-apple-darwin10/4.0.1/include/ -I$(SDKROOT)/usr/include/
CFLAGS=$(CPPFLAGS) -pipe -no-cpp-precomp -isysroot $(SDKROOT)
CPP="/usr/bin/cpp $(CPPFLAGS)"

CC=$(DEVROOT)/usr/bin/i686-apple-darwin10-gcc-4.0.1
LD=$(DEVROOT)/usr/bin/ld
LDFLAGS= -Z -L$(SDKROOT)/usr/lib/gcc/i686-apple-darwin10/4.0.1/include/ -L$(SDKROOT)/usr/include/ -isysroot $(SDKROOT) -Wl,-dead_strip
LIBRARY=1


CC_OPTIMIZE	= -O3 # default C compiler optimization flags
#CC_OPTIMIZE	= -O3 -msse2 -mfpmath=sse -fomit-frame-pointer # optimize for speed on a Pentium 4 or higher CPU

VERSION		= `cat VERSION`
OPTFLAGS	?= $(CC_OPTIMIZE) -Wall -Wshadow -Wno-char-subscripts # optional gcc specific flags
CFLAGS		+= $(OPTFLAGS)
CFLAGS		+= -DUNIX -DVERSION=\"$(VERSION)\"
LDLIBS		+= -lm # libraries to link with to create the Mathomatic executable

# "make READLINE=1" to include readline support:
CFLAGS		+= $(READLINE:1=-DREADLINE)
LDLIBS		+= $(READLINE:1=-lreadline -lncurses)

# Uncomment the following to force generation of x86-64-bit code:
#CFLAGS		+= -m64

# Install directories follow; installs everything in $(DESTDIR)/usr/local by default.
# Note that support for the DESTDIR variable was recently added in version 15.2.1.
# DESTDIR is only used by package maintainers, who should remove all DESTDIR patches.
prefix		?= /usr/local
bindir		?= $(prefix)/bin
mandir		?= $(prefix)/share/man
docdir		?= $(prefix)/share/doc
mathdocdir	?= $(docdir)/mathomatic

# Mathomatic program names (can be changed):
AOUT		= mathomatic
M4SCRIPTNAME	= matho
M4SCRIPTPATH	= $(DESTDIR)$(bindir)/$(M4SCRIPTNAME)

INCLUDES	= includes.h license.h am.h externs.h complex.h proto.h altproto.h
OBJECTS		= main.o globals.o am.o solve.o help.o parse.o cmds.o simplify.o \
		  factor.o super.o unfactor.o poly.o diff.o integrate.o \
		  complex.o complex_lib.o list.o gcd.o factor_int.o

PRIMES_MANHTML	= doc/matho-primes.1.html doc/matho-pascal.1.html doc/matho-sumsq.1.html \
		  doc/primorial.1.html doc/matho-mult.1.html doc/matho-sum.1.html
# HTML man pages to make:
MANHTML		= doc/mathomatic.1.html $(PRIMES_MANHTML)

# Flags to make HTML man pages with rman:
RMANOPTS	= -f HTML -r '%s.%s.html'

# man pages to install:
MAN1		= mathomatic.1
# Text files to install:
DOCS		= VERSION AUTHORS COPYING README.txt changes.txt

PDFDOC		= doc/manual.pdf

all: $(AOUT) html # make these by default

lib $(LIB): $(OBJECTS)
	ar cr $(LIB) $(OBJECTS)
	ranlib $(LIB)

doc html: $(MANHTML)

# TEST MATHOMATIC

# Run "make test" to see if the resulting mathomatic executable runs properly on your system.
# It does a diff between the output of the test and the expected output.
# If no differences are reported, "All tests passed" is displayed.
check test:
	@echo
	@echo Testing ./$(AOUT)
	cd tests && time -p ../$(AOUT) -t all 0<&- >test.out && diff -u all.out test.out && rm test.out && cd ..
	@echo
	@echo All tests passed.

# "make baseline" generates the expected output file for "make test".
# Do not run this unless you are sure Mathomatic is working correctly
# and you need "make test" to succeed with no errors.
baseline tests/all.out:
	cd tests && ../$(AOUT) -t all 0<&- >all.out && cd ..
	@rm -f tests/test.out
	@echo
	@echo File tests/all.out updated with current test output.

# BUILD MATHOMATIC

proto.h:
	./update # shell script to update proto.h

$(OBJECTS): $(INCLUDES) VERSION

# Create the mathomatic executable:
$(AOUT): $(OBJECTS)
	$(CC) $(LDFLAGS) $(CFLAGS) $+ $(LDLIBS) -o $(AOUT)
	@echo
	@echo ./$(AOUT) created.

# To compile Mathomatic as a stand-alone executable
# that has no shared library dependencies, type "make clean static".
static: $(OBJECTS)
	$(CC) $(LDFLAGS) $(CFLAGS) $+ -static $(LDLIBS) -o $(AOUT)
	@echo
	@echo ./$(AOUT) created.


# BUILD THE MATHOMATIC DOCUMENTATION

# Create the PDF version of the Mathomatic documentation, if desired.
# Requires the htmldoc program be installed.
pdf $(PDFDOC): doc/manual.html doc/am.html doc/mathomatic.1.html
	htmldoc --webpage --format pdf --linkstyle plain --no-links --title --logoimage icons/mathomatic.png --footer h1l --header "c d" -f $(PDFDOC) $+
	@echo PDF documentation $(PDFDOC) generated.

# Here we convert the man pages to HTML docs with rman:
doc/mathomatic.1.html: mathomatic.1
	@rman --version # display version number and test for existence
	rman $(RMANOPTS) $+ >$@

$(PRIMES_MANHTML): doc/%.1.html: primes/%.1
	@rman --version
	rman $(RMANOPTS) $< >$@

# INSTALL MATHOMATIC

# The following installs shell scripts allowing convenient entry of trig functions for Mathomatic.
m4install: install
	echo '#!/bin/sh' >$(M4SCRIPTPATH)
	echo '# Shell script to run Mathomatic with the m4 preprocessor.' >>$(M4SCRIPTPATH)
	echo '# This allows entry of many standard math functions.' >>$(M4SCRIPTPATH)
	echo '#' >>$(M4SCRIPTPATH)
	echo '# Usage: $(M4SCRIPTNAME) [ input_files ]' >>$(M4SCRIPTPATH)
	echo >>$(M4SCRIPTPATH)
	echo 'm4 -eP -- $(mathdocdir)/m4/functions.m4 "$$@" - | time -p mathomatic -ru' >>$(M4SCRIPTPATH)
	chmod 0755 $(M4SCRIPTPATH)
	$(INSTALL_PROGRAM) m4/rmath $(DESTDIR)$(bindir)
	@echo
	@echo m4 Mathomatic install completed.
	@echo Type \"rmath\" to run m4 Mathomatic.
	@echo

# Install the binaries and documentation.
install: bininstall docinstall
	@echo
	@echo Mathomatic is installed.
	@echo Type \"$(AOUT)\" to run Mathomatic.
	@echo

# Install stripped binaries and documentation.
install-strip:
	$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' install

bininstall:
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) -d $(DESTDIR)$(prefix)/share/applications
	$(INSTALL) -d $(DESTDIR)$(prefix)/share/pixmaps
	$(INSTALL_PROGRAM) $(AOUT) $(DESTDIR)$(bindir)
	$(INSTALL_DATA) icons/mathomatic.desktop $(DESTDIR)$(prefix)/share/applications
	$(INSTALL_DATA) icons/mathomatic.png $(DESTDIR)$(prefix)/share/pixmaps
	$(INSTALL_DATA) icons/mathomatic.xpm $(DESTDIR)$(prefix)/share/pixmaps

docinstall:
	$(INSTALL) -d $(DESTDIR)$(mandir)/man1
	$(INSTALL) -d $(DESTDIR)$(mathdocdir)
	$(INSTALL) -d $(DESTDIR)$(mathdocdir)/html
	$(INSTALL) -d $(DESTDIR)$(mathdocdir)/m4
	$(INSTALL) -d $(DESTDIR)$(mathdocdir)/tests
	$(INSTALL) -d $(DESTDIR)$(mathdocdir)/factorial
	$(INSTALL_DATA) $(MAN1) $(DESTDIR)$(mandir)/man1
	$(INSTALL_DATA) $(DOCS) $(DESTDIR)$(mathdocdir)
	$(INSTALL_DATA) doc/* $(DESTDIR)$(mathdocdir)/html
	$(INSTALL_DATA) m4/* $(DESTDIR)$(mathdocdir)/m4
	$(INSTALL_DATA) tests/* $(DESTDIR)$(mathdocdir)/tests
	$(INSTALL_DATA) factorial/* $(DESTDIR)$(mathdocdir)/factorial

# UNINSTALL MATHOMATIC

uninstall:
	cd $(DESTDIR)$(mandir)/man1 && rm -f $(MAN1)
	rm -f $(DESTDIR)$(bindir)/$(AOUT)
	rm -rf $(DESTDIR)$(mathdocdir)
	rm -f $(DESTDIR)$(prefix)/share/applications/mathomatic.desktop
	rm -f $(DESTDIR)$(prefix)/share/pixmaps/mathomatic.*
	rm -f $(DESTDIR)$(bindir)/rmath $(M4SCRIPTPATH)
	@echo
	@echo Uninstall completed.

# CLEAN THIS DIRECTORY

clean:
	rm -f *.o *.a
	rm -f */*.o */*.a */*.pyc
	rm -f tests/test.out primes/test.out

distclean: clean
	rm -f $(AOUT)
	rm -f mathomatic_secure
	rm -f $(PDFDOC)
	$(MAKE) -C lib distclean
	$(MAKE) -C primes distclean

maintainer-clean flush: distclean
	rm -f $(MANHTML)
