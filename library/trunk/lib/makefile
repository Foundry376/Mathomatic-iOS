DEVROOT=/Developer/Platforms/iPhoneOS.platform/Developer
SDKROOT=$(DEVROOT)/SDKs/iPhoneOS4.1.sdk

# Export some things for the ARM architecture
CPPFLAGS=-I$(SDKROOT)/usr/lib/gcc/arm-apple-darwin10/4.0.1/include/ -I$(SDKROOT)/usr/include/
CFLAGS=$(CPPFLAGS) -arch armv6 -pipe -no-cpp-precomp -isysroot $(SDKROOT)
CPP="/usr/bin/cpp $(CPPFLAGS)"

CC=$(DEVROOT)/usr/bin/arm-apple-darwin10-gcc-4.0.1
LD=$(DEVROOT)/usr/bin/ld
LDFLAGS= -Z -L$(SDKROOT)/usr/lib/gcc/arm-apple-darwin10/4.0.1/include/ -L$(SDKROOT)/usr/include/ -isysroot $(SDKROOT) -miphoneos-version-min=2.1 -Wl,-dead_strip
LIBRARY=1

# Makefile for the Mathomatic symbolic math library and its test program.
# See file README.txt for instructions.

SHELL		= /bin/sh # from http://www.gnu.org/prep/standards/
CC		?= gcc # C compiler to use
INSTALL		?= install # installer to use
INSTALL_PROGRAM	?= $(INSTALL) # command to install executable program files
INSTALL_DATA	?= $(INSTALL) -m 0644 # command to install data files

VERSION		= `cat ../VERSION`
OPTFLAGS	?= -O3 -Wall -Wshadow -Wno-char-subscripts # gcc specific flags; can be removed
CFLAGS		+= $(OPTFLAGS)
CFLAGS		+= -DLIBRARY -DVERSION=\"$(VERSION)\" # necessary C compiler flags
LDLIBS		+= -lm # system libraries to link

# Install directories follow; installs everything in $(DESTDIR)/usr/local by default.
prefix		?= /usr/local
libdir		?= $(prefix)/lib
includedir	?= $(prefix)/include

AOUT		= testmain # The name of the library test executable file to create.
LIB		= libmathomatic.a # The name of the symbolic math library file to create.
HEADERS		= mathomatic.h

OBJECTS		= globals.o am.o solve.o help.o parse.o cmds.o simplify.o \
		  factor.o super.o unfactor.o poly.o diff.o integrate.o \
		  complex.o complex_lib.o list.o gcd.o factor_int.o

all: $(LIB) $(AOUT)

lib $(LIB): lib.o $(OBJECTS)
	$(AR) cr $(LIB) $+
	-ranlib $(LIB)
	@echo
	@echo Symbolic math library $(LIB) created.
	@echo

lib.o $(OBJECTS): $(HEADERS) ../includes.h ../license.h ../am.h ../externs.h ../complex.h ../proto.h ../altproto.h ../VERSION

$(OBJECTS): %.o: ../%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

$(AOUT): testmain.o $(LIB)
	$(CC) $(LDFLAGS) $(CFLAGS) $+ $(LDLIBS) -o $(AOUT)
	@echo
	@echo ./$(AOUT) created.

install:
	$(INSTALL) -d $(DESTDIR)$(libdir)
	$(INSTALL) -d $(DESTDIR)$(includedir)
	$(INSTALL_DATA) $(LIB) $(DESTDIR)$(libdir)
	$(INSTALL_DATA) $(HEADERS) $(DESTDIR)$(includedir)
	@echo
	@echo Mathomatic symbolic math library installed.

uninstall:
	cd $(DESTDIR)$(includedir) && rm -f $(HEADERS)
	rm -f $(DESTDIR)$(libdir)/$(LIB)
	@echo
	@echo Uninstall completed.

clean:
	rm -f *.o

distclean maintainer-clean flush: clean
	rm -f $(AOUT)
	rm -f *.a
