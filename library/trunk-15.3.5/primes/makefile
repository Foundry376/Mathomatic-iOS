# Makefile for compiling and installing the Mathomatic Prime Number Tools
# under a Unix-like system.
# See file README.txt for instructions.
#
# Uses the Gnu99 standard; may require "-std=gnu99" on the gcc command line
# if using an older compiler.

SHELL		= /bin/sh # from http://www.gnu.org/prep/standards/
CC		?= gcc # C compiler to use
INSTALL		?= install # installer to use
INSTALL_PROGRAM	?= $(INSTALL) # command to install executable program files
INSTALL_DATA	?= $(INSTALL) -m 0644 # command to install data files

CC_OPTIMIZE	= -O3 # default C compiler optimization flags
#CC_OPTIMIZE	= -O3 -ffast-math -msse2 -mfpmath=sse -fomit-frame-pointer # optimize for speed on a Pentium 4 or higher CPU

OPTFLAGS	?= $(CC_OPTIMIZE) -Wall -Wshadow -Wno-char-subscripts # gcc specific flags; comment out for other C compilers.
CFLAGS		+= $(OPTFLAGS)
LDLIBS		+= -lm

# Install directories follow, installs everything in $(DESTDIR)/usr/local by default.
prefix		?= /usr/local
bindir		?= $(prefix)/bin
mandir		?= $(prefix)/share/man

# The executables to create:
TARGETS		= matho-primes matho-pascal matho-sumsq
# The Python scripts to install:
PYTHON_SCRIPTS	= primorial matho-mult matho-sum
# The executables to install:
EXECUTABLES	= $(TARGETS) $(PYTHON_SCRIPTS)
# The man pages to install:
MAN1		= matho-primes.1 matho-pascal.1 matho-sumsq.1 primorial.1 matho-mult.1 matho-sum.1

all: $(TARGETS) # make these by default

matho-sumsq: matho-sumsq.o lsqrt.o
	$(CC) $(LDFLAGS) $(CFLAGS) $+ $(LDLIBS) -o $@

check test:
	@echo Test basic functionality of matho-primes:
	time -p ./matho-primes 10000000000000 10000000300000 twin >test.out && diff -u twins.out test.out
	@rm test.out
	@echo Test passed.
	@echo

bigtest:
	@echo Test that long doubles are actually used:
	time -p ./matho-primes 100000000000000000 100000000000300000 twin >test.out && diff -u bigtwins.out test.out
	@rm test.out
	@echo Test passed.
	@echo

install:
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) -d $(DESTDIR)$(mandir)/man1
	$(INSTALL_PROGRAM) $(EXECUTABLES) $(DESTDIR)$(bindir)
	$(INSTALL_DATA) $(MAN1) $(DESTDIR)$(mandir)/man1
	@echo
	@echo The Prime Number Tools are installed.

uninstall:
	cd $(DESTDIR)$(bindir) && rm -f $(EXECUTABLES)
	cd $(DESTDIR)$(mandir)/man1 && rm -f $(MAN1)
	@echo
	@echo Uninstall completed.

clean flush distclean maintainer-clean:
	rm -f *.o *.a
	rm -f $(TARGETS)
	rm -f test.out
