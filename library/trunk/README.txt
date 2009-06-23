
                                   Mathomatic
                                       by
                               George Gesslein II

This directory contains the complete C source code and documentation for
Mathomatic, the automatic algebraic manipulator.  Using the included
makefiles, this text mode application should compile with gcc and run
correctly under Unix, Linux, Mac OS X, and Cygwin, without any modifications.
Most of the Mathomatic code can also be called from any C or C++ program,
when linked with Mathomatic specially compiled as a symbolic math library.

Mathomatic is a portable Computer Algebra System (CAS) and calculator
software that can symbolically solve, simplify, combine, and compare
equations; perform complex number and polynomial arithmetic, etc.  It does
some calculus and is very easy to compile and use.

All software and documentation in this archive is copyrighted and made
available under the GNU Lesser General Public License (LGPL) version 2.1 (see
file "COPYING").  This means Mathomatic is free and comes with no warranty at
all, but if you find any bugs in the software or algorithms, I will try to
fix them.


                                  Compilation
                                  -----------

This section can be skipped; you may wish to jump to the next section with
short compile/install instructions, or read the file "INSTALL.txt", if you
only wish to install Mathomatic.

To compile under Mac OS X, Xcode Tools should be installed, which contains
the GNU C compiler and make utility.  Xcode Tools is available on one of the
Mac OS X installation discs.  To open a shell window, click on the "Terminal"
application in "Applications/Utilities".

To compile Mathomatic without readline support, type "make" at a shell prompt
while in the Mathomatic source directory.  This will compile the C source
code to create the executable file named "mathomatic".  To run Mathomatic,
type "./mathomatic" at the shell prompt.

To test most functionality, type:

	make test

To recompile with readline editing and history of all Mathomatic input, type:

	make clean
	make READLINE=1

This allows use of the cursor keys to recall and edit previously entered and
pushed expressions when running Mathomatic.  The GNU readline library must be
installed to use readline.

To create the secure version of Mathomatic, with no file I/O nor shelling out
possible, type:

	./compile.secure

This will create the executable "mathomatic_secure", which can safely be used
as a telnet application or CGI program.  The "mathomatic -s" command line
option functions similarly, though is not as secure.


                                  Installation
                                  ------------

A typical installation is done by typing the following at the shell prompt:

	make READLINE=1
	make test
	sudo make install

This will compile the source code, verify the functionality of the generated
executable (named "mathomatic"), and install the executable, docs, and tests
in "/usr/local" in less than a minute.  If you like, use "make m4install"
instead of "make install", to install m4 Mathomatic also, which are shell
scripts named "matho" and "rmath".  m4 (macro) Mathomatic allows easy entry
of math functions like sqrt(x) and sin(x) as macros.  "matho" runs m4
Mathomatic without readline support and "rmath" runs "matho" with a readline
front end, if the "rlwrap" program is available.

There are quite a few math goodies in this archive, besides the main
Mathomatic program:

  The directory "icons" contains Mathomatic icons for your desktop.
  The directory "doc" contains the Mathomatic documentation in HTML.
  The directory "tests" contains Mathomatic test and example scripts.
  The directory "primes" contains a prime number generator utility, etc.
  The directory "lib" contains the API and test for the Mathomatic library.
  The directory "m4" contains m4 Mathomatic supporting files.
  The directory "misc" contains a polynomial equation solver utility.

For quick help while running Mathomatic, type "?" or use the help command.
To read or print the documentation, point your web browser to the file
"doc/index.html", or "/usr/local/share/doc/mathomatic/html/index.html" if
Mathomatic was installed.  When copying the Mathomatic documentation, please
copy the entire html directory, not just selected files from it.

For the most recent source code, binaries, documentation, information, and
other help, please visit the Mathomatic website: http://mathomatic.org

Author and maintainer contact information:

  email:
    georgegesslein@gmail.com

  postal address:
    George Gesslein II
    43 Atwater Road
    Lansing, New York 14882
    USA

The author is happy to help friendly people with any problems using this
software.  If you get stuck, send email to him.

Please report suspected bugs in Mathomatic on the Ubuntu Launchpad website:
"http://launchpad.net/mathomatic".  Mathomatic should always give correct
answers; if not, report it.


             Compile-time defines for the Mathomatic source code
             ---------------------------------------------------

To compile Mathomatic for UNIX, GNU/Linux, Mac OS X, or any POSIX compliant
OS, define "UNIX" (see "makefile").  To compile Mathomatic for a generic
system, simply compile with no defines.  To compile for Microsoft Windows
using Cygwin, define only "CYGWIN" (see "makefile.cygwin").  To compile for
hand-helds like the Nintendo DS, define "HANDHELD" and "SECURE" for greatly
reduced memory usage and no file I/O.

Define "READLINE" and include the readline libraries at link time to use
readline mode.  This will allow easy command line editing and history.  The
GNU readline development package must be installed to compile with this
option and the readline library must be installed to run the resulting
executable.

Define "SILENT" to remove all helpful messages, warnings, and debugging code.
This is useful when using Mathomatic in another program or if you only want
terse output.  The Mathomatic command "set debug -1" is another way to not
display helpful messages and warnings.

Define "LIBRARY" when using the Mathomatic code as a symbolic math library.
"SILENT" is automatically defined when this is defined.  Most standard input
and output is disabled.  See the directory "lib" and the file "makefile.lib"
for the library hooks (API) and test program and how to compile it.  The
following commands are omitted with this option: calculate, edit, optimize,
pause, quit, and tally.

Define "SECURE" to disable all file reading, writing, and executing.  This is
useful when making Mathomatic available to the public through telnet or CGI
programs.  It is also useful when making ROMable code.  All insecure commands
and code are omitted with this option.  See "compile.secure", which is the
secure Mathomatic build script.

Define "TIMEOUT_SECONDS" to set the maximum number of seconds Mathomatic may
run.  Upon timeout, Mathomatic properly exits.  This is useful when making
Mathomatic a telnet or CGI program, so it won't overload the server.

The "I18N" define is meant to enable internationalization using gettext(3),
however the actual code to do this has not been implemented, so do not define
"I18N" until it is.  Currently all strings to be translated have been marked,
but no translations have been made, so Mathomatic is only available in
English.

To see which of the above defines were used in a compiled version of
Mathomatic, use the version command.


                         Mathomatic C source code files
                         ------------------------------

  altproto.h - alternate proto.h
  am.h - the main include file for Mathomatic, contains tunable parameters
  complex.h - floating point complex number arithmetic function prototypes
  externs.h - global variable extern definitions, from globals.c
  includes.h - automatically includes all necessary include files
  license.h - the current Mathomatic license
  proto.h - global function prototypes, made with cproto utility

  am.c - miscellaneous C functions for Mathomatic
  cmds.c - code for commands that don't belong anywhere else
  complex.c - floating point complex number routines for Mathomatic
  complex_lib.c - generic floating point complex number arithmetic library
  diff.c - symbolic differentiation routines and related commands
  factor.c - symbolic factorizing routines (not polynomial factoring)
  factor_int.c - floating point constant factorizing routines
  gcd.c - floating point GCD and numerical fractions code
  globals.c - global variable and array definitions, duped in externs.h
  help.c - command table, help command, and input parsing routines
  integrate.c - integration routines and commands
  list.c - expression and equation display routines
  main.c - startup code for Mathomatic, not used for library
  parse.c - mathematical expression parsing routines
  poly.c - simplifying and polynomial routines
  simplify.c - simplifying routines
  solve.c - symbolic solving routines
  super.c - group and combine denominators of symbolic fractions
  unfactor.c - symbolic unfactorizing (expanding) routines
