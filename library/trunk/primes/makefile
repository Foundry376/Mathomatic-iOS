# Makefile for compiling and installing the Mathomatic Prime Number Tools under Linux and Unix.
# gcc is the C compiler used.

CFLAGS		+= -O2 -Wall -std=gnu99
LDFLAGS		+= -lm

prefix		?= /usr/local
bindir		?= $(prefix)/bin
mandir		?= $(prefix)/share/man

TARGETS		= matho-primes matho-pascal matho-sumsq # the executables
MAN1		= matho-primes.1 matho-pascal.1 matho-sumsq.1 # the man pages

all: $(TARGETS)
	@echo Make completed.

matho-sumsq: matho-sumsq.o lsqrt.o

check test:
	time ./matho-primes 10000000000000 10000000300000 twin >test.out && diff -u twins.out test.out
	@rm test.out
	@echo Test passed.

bigtest:
	time ./matho-primes 1000000000000000000 1000000000000300000 twin >test.out && diff -u bigtwins.out test.out
	@rm test.out
	@echo Test passed.

install:
	install -d $(bindir)
	install -d $(mandir)/man1
	install -m 0755 $(TARGETS) $(bindir)
	install -m 0644 $(MAN1) $(mandir)/man1
	@echo Install completed.

uninstall:
	cd $(bindir) && rm -f $(TARGETS)
	cd $(mandir)/man1 && rm -f $(MAN1)

clean:
	rm -f *.o
	rm -f $(TARGETS)
	rm -f test.out
