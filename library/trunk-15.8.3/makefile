# Makefile for compiling, testing, and installing Mathomatic and its documentation.
# Requires the GNU make utility, in BSD Unix this is called "gmake".
# Remove the -DUNIX define in CFLAGS when not using Mathomatic for desktop UNIX/Linux.

# To compile, test, and install all of Mathomatic under CygWin for MS-Windows:
# 	CFLAGS="-DCYGWIN -DBOLD_COLOR -O3" make READLINE=1
#	make test
#	make m4install

# To compile Mathomatic with the MinGW cross-compiler for MS-Windows:
#	./compile.mingw

# For normal compilation and testing on a GNU system,
# please type the following at the shell prompt:
#	make clean
# 	make READLINE=1
# 	make test
#
# For Slackware, or any other OS that fails to find the curses library functions, use:
#	LDLIBS=-lncurses make READLINE=1

# To install the base Mathomatic system after compiling and testing, type:
#	sudo make install
#
# To install all of Mathomatic, including m4 Mathomatic, instead type:
#	sudo make m4install

# To compile, test, and install all of Mathomatic under OpenIndiana (SunOS), type:
#	LDLIBS=-lcurses make READLINE=1
#	make test
#	sudo make m4install INSTALL=ginstall prefix=/usr M4=gm4

# This makefile does NOT compile and install the Prime Number Tools in the "primes" directory,
# nor does it compile and install the Symbolic Math Library in the "lib" directory.
#	sudo make uninstall
# here will uninstall all of Mathomatic,
# including the Prime Number Tools and the Symbolic Math Library,
# but it must know the "prefix" used to install Mathomatic, if specified during the install.

SHELL		= /bin/sh # from "http://www.gnu.org/prep/standards/", do not change
CC		?= gcc # C compiler to use; this statement doesn't work usually, instead using cc.
M4		?= m4 # Change this to gm4 in Unix or a non-GNU system.
INSTALL		?= install # Installer utility to use; change to ginstall under Unix.
INSTALL_PROGRAM	?= $(INSTALL) # Command to install executable program files.
INSTALL_DATA	?= $(INSTALL) -m 0644 # Command to install data files.

CC_OPTIMIZE	?= -O3 # Default C compiler optimization flags that are safe.
# Be sure and run tests to see if Mathomatic works and runs faster, if you uncomment the following line:
#CC_OPTIMIZE	+= -fno-signaling-nans -fno-trapping-math -fomit-frame-pointer # Possible additional optimizations, not tested.

VERSION		= `cat VERSION`
OPTFLAGS	?= $(CC_OPTIMIZE) -Wall -Wshadow -Wno-char-subscripts # optional gcc specific flags
CFLAGS		?= $(OPTFLAGS)
CFLAGS		+= -fexceptions -DUNIX -DVERSION=\"$(VERSION)\"
LDLIBS		+= -lm # libraries to link with to create the Mathomatic executable

# Run "make READLINE=1" to include the optional readline editing and history support:
CFLAGS		+= $(READLINE:1=-DREADLINE)
LDLIBS		+= $(READLINE:1=-lreadline) # Add -lncurses if needed for readline, might be called "curses" on some systems.

# Uncomment the following line to force generation of x86-64-bit code:
#CFLAGS		+= -m64

# Install directories follow; installs everything in $(DESTDIR)/usr/local by default.
# Note that support for the DESTDIR variable was added in version 15.2.1.
# DESTDIR is used by package maintainers, who should remove all DESTDIR patches,
# because DESTDIR is handled properly (according to GNU standards) by this makefile now.
prefix		?= /usr/local
bindir		?= $(prefix)/bin
datadir		?= $(prefix)/share
mandir		?= $(prefix)/share/man
docdir		?= $(prefix)/share/doc
mathdocdir	?= $(docdir)/mathomatic
mathdatadir	?= $(datadir)/mathomatic

# Mathomatic program names:
AOUT		?= mathomatic
M4SCRIPTPATH	= $(DESTDIR)$(bindir)/matho

INCLUDES	= includes.h license.h standard.h am.h externs.h complex.h proto.h altproto.h
MATHOMATIC_OBJECTS += main.o globals.o am.o solve.o help.o parse.o cmds.o simplify.o \
		  factor.o super.o unfactor.o poly.o diff.o integrate.o \
		  complex.o complex_lib.o list.o gcd.o factor_int.o

PRIMES_MANHTML	= doc/matho-primes.1.html doc/matho-pascal.1.html doc/matho-sumsq.1.html \
		  doc/primorial.1.html doc/matho-mult.1.html doc/matho-sum.1.html
# HTML man pages to make:
MANHTML		= doc/mathomatic.1.html doc/rmath.1.html $(PRIMES_MANHTML)

# Flags to make HTML man pages with rman:
RMANOPTS	= -f HTML -r '%s.%s.html'
RMAN		= rman
# Flags to make HTML man pages with groff instead (uncomment below and comment above to use):
#RMANOPTS       = -man -Thtml -P -l 
#RMAN		= groff

# man pages to install:
MAN1		= mathomatic.1 rmath.1
# Text files to install:
TEXT_DOCS	?= VERSION AUTHORS COPYING README.txt NEWS

PDFDOC		= manual.pdf	# Name of PDF file containing the Mathomatic User Guide.
PDFMAN		= bookman.pdf	# Name of PDF file containing all the Mathomatic man pages.

all static: $(AOUT) html NEWS # make these by default

# Make sure the HTML man pages are up-to-date.
html: $(MANHTML)

# Make the Quick Reference Card.
htmlcard doc/quickrefcard.html: $(AOUT)
	./makehtmlcard.sh doc/

# Make the PDF cheat sheet.
pdfsheet quickref.pdf: $(AOUT)
	./makepdfsheet.sh doc/

# TEST MATHOMATIC

# Run "make test" to see if the resulting mathomatic executable runs properly on your system.
# It does a diff between the output of the test and the expected output.
# If no differences are reported, "All tests passed" is displayed.
test:
	@echo
	@echo Testing ./$(AOUT)
	cd tests && time -p ../$(AOUT) -t all 0<&- >test.out && diff -u --strip-trailing-cr all.out test.out && rm test.out && cd ..
	@echo
	@echo All tests passed.

# Same as "make test", except it doesn't run the time command.
check:
	@echo
	@echo Testing ./$(AOUT)
	cd tests && ../$(AOUT) -t all 0<&- >test.out && diff -u --strip-trailing-cr all.out test.out && rm test.out && cd ..
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
	./update # Shell script to update proto.h using the cproto utility.

$(MATHOMATIC_OBJECTS): $(INCLUDES) VERSION

# To compile Mathomatic as a stand-alone executable
# that has no shared library dependencies, type "make clean static".
static: LDFLAGS += -static

# Create the mathomatic executable:
$(AOUT): $(MATHOMATIC_OBJECTS)
	$(CC) $(LDFLAGS) $(CFLAGS) $+ $(LDLIBS) -o $(AOUT)
	@echo
	@echo ./$(AOUT) successfully created.

# BUILD THE MATHOMATIC DOCUMENTATION

# Build the nicely ordered changelog.
NEWS: changes.txt
	./makenews.sh

# "make pdfdoc" creates the PDF version of the Mathomatic User Guide, if desired.
# Requires the htmldoc program be installed.
pdf pdfdoc $(PDFDOC): doc/manual.html doc/am.html doc/fdl-1.3-standalone.html
	htmldoc --webpage --format pdf --linkstyle plain --no-links --title --logoimage icons/mathomatic.png --footer h1l --header "c d" -f $(PDFDOC) $+
	@echo PDF documentation $(PDFDOC) generated.

# "make pdfman" creates a PDF of all the Mathomatic Man pages, if desired.
# Requires the very latest version of the txt2man package.
bookman pdfman $(PDFMAN): $(MAN1) primes/*.1 lib/*.3
	bookman -p -t "Mathomatic version $(VERSION) Man Pages" -a "George Gesslein II" -d `date +%F` $+ >$(PDFMAN) </dev/null
	@echo PDF man pages book created in $(PDFMAN)

# Here we convert the man pages to HTML docs with rman:
doc/mathomatic.1.html: mathomatic.1
	@$(RMAN) --version $(RMANOPTS) >/dev/null # Test for existence of commands.
	@tidy -version >/dev/null
	-$(RMAN) $(RMANOPTS) $< | tidy -q -i -file /dev/null >$@
	@echo Successfully created $@
	wc $@

doc/rmath.1.html: rmath.1
	@$(RMAN) --version $(RMANOPTS) >/dev/null # Test for existence of commands.
	@tidy -version >/dev/null
	-$(RMAN) $(RMANOPTS) $< | tidy -q -i -file /dev/null >$@
	@echo Successfully created $@
	wc $@

$(PRIMES_MANHTML): doc/%.1.html: primes/%.1
	@$(RMAN) --version $(RMANOPTS) >/dev/null # Test for existence of commands.
	@tidy -version >/dev/null
	-$(RMAN) $(RMANOPTS) $< | tidy -q -i -file /dev/null >$@
	@echo Successfully created $@
	wc $@

# INSTALL MATHOMATIC

# The following installs everything.
m4install: install matho-rmath-install
	@echo
	@echo m4 Mathomatic install completed.
	@echo

# The following installs shell scripts matho and rmath,
# allowing convenient entry of standard math functions.
# Requires at least "make bininstall" first.
matho-rmath-install:
	$(INSTALL) -d $(DESTDIR)$(mathdatadir)
	$(INSTALL) -d $(DESTDIR)$(mathdatadir)/m4
	echo '#!/bin/sh' >$(M4SCRIPTPATH)
	echo '# Shell script to run Mathomatic with the GNU m4 preprocessor.' >>$(M4SCRIPTPATH)
	echo '# This allows entry of many standard math functions.' >>$(M4SCRIPTPATH)
	echo '#' >>$(M4SCRIPTPATH)
	echo '# Usage: matho [ input_files ]' >>$(M4SCRIPTPATH)
	echo >>$(M4SCRIPTPATH)
	echo 'if ! $(M4) --version >/dev/null' >>$(M4SCRIPTPATH)
	echo 'then' >>$(M4SCRIPTPATH)
	echo '  echo The $(M4) package is not installed.  GNU m4 is required to run m4 Mathomatic.' >>$(M4SCRIPTPATH)
	echo '  exit 1' >>$(M4SCRIPTPATH)
	echo 'fi' >>$(M4SCRIPTPATH)
	echo >>$(M4SCRIPTPATH)
	echo '$(M4) -eP -- $(mathdatadir)/m4/functions.m4 "$$@" - | mathomatic -ru -s-1' >>$(M4SCRIPTPATH)
	chmod 0755 $(M4SCRIPTPATH)
	$(INSTALL_PROGRAM) m4/rmath $(DESTDIR)$(bindir)
	$(INSTALL_DATA) rmath.1 $(DESTDIR)$(mandir)/man1
	cd $(DESTDIR)$(mandir)/man1 && ln -sf rmath.1 matho.1
	$(INSTALL_DATA) m4/functions.m4 $(DESTDIR)$(mathdatadir)/m4

# Install m4 Mathomatic with trig functions that use degree units, instead of radians.
matho-rmath-install-degrees: matho-rmath-install
	cat m4/degrees.m4 >>$(DESTDIR)$(mathdatadir)/m4/functions.m4
	@echo Degree mode installed.

# Install complete m4 Mathomatic with trig functions that use degree units, instead of radians.
m4install-degrees: m4install
	cat m4/degrees.m4 >>$(DESTDIR)$(mathdatadir)/m4/functions.m4
	@echo Degree mode installed.

# Install the Mathomatic binaries and documentation.  Does not install m4 Mathomatic.
install: bininstall docinstall
	@echo
	@echo Mathomatic is installed.
	@echo

# Strip the binaries of their symbol tables.  Makes smaller executables, but debugging becomes impossible.
strip: $(AOUT)
	strip $(AOUT)

# Binaries only install.
bininstall:
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) -d $(DESTDIR)$(datadir)/applications
	$(INSTALL) -d $(DESTDIR)$(datadir)/pixmaps
	$(INSTALL) -d $(DESTDIR)$(mandir)/man1
	$(INSTALL_PROGRAM) $(AOUT) $(DESTDIR)$(bindir)
	$(INSTALL_DATA) icons/mathomatic.desktop $(DESTDIR)$(datadir)/applications
	$(INSTALL_DATA) icons/mathomatic.png icons/mathomatic.xpm $(DESTDIR)$(datadir)/pixmaps
	$(INSTALL_DATA) mathomatic.1 $(DESTDIR)$(mandir)/man1

# Documentation only install.
docinstall:
	@echo Installing docs...
	$(INSTALL) -d $(DESTDIR)$(mathdocdir)
	$(INSTALL) -d $(DESTDIR)$(mathdocdir)/html
	$(INSTALL) -d $(DESTDIR)$(mathdocdir)/tests
	$(INSTALL) -d $(DESTDIR)$(mathdocdir)/examples
	$(INSTALL_DATA) $(TEXT_DOCS) $(DESTDIR)$(mathdocdir)
	$(INSTALL_DATA) doc/* $(DESTDIR)$(mathdocdir)/html
	-$(INSTALL_DATA) *.pdf $(DESTDIR)$(mathdocdir)
	$(INSTALL_DATA) tests/* $(DESTDIR)$(mathdocdir)/tests
	$(INSTALL_DATA) examples/* $(DESTDIR)$(mathdocdir)/examples

# UNINSTALL MATHOMATIC

# Entirely remove Mathomatic from your local computer system.
uninstall:
	cd $(DESTDIR)$(mandir)/man1 && rm -f $(MAN1) matho.1
	cd $(DESTDIR)$(bindir) && rm -f $(AOUT) rmath matho
	rm -rf $(DESTDIR)$(mathdocdir)
	rm -rf $(DESTDIR)$(mathdatadir)
	rm -f $(DESTDIR)$(datadir)/applications/mathomatic.desktop
	rm -f $(DESTDIR)$(datadir)/pixmaps/mathomatic.*
	@echo
	@echo Mathomatic uninstall completed.
	@echo
	-$(MAKE) -C primes prefix=$(prefix) uninstall
	@echo
	-$(MAKE) -C lib prefix=$(prefix) uninstall

# CLEAN THIS DIRECTORY

clean:
	rm -f *.o
	rm -f */*.o */*.pyc */*.pyo
	rm -f tests/test.out primes/test.out primes/bigtest.out

distclean: clean
	rm -f *.a */*.a
	rm -f $(AOUT)
	rm -f mathomatic_secure
	rm -f *.exe
	rm -f *.pdf
	$(MAKE) -C primes distclean
	$(MAKE) -C lib distclean

maintainer-clean flush: distclean
	rm -f $(MANHTML)
