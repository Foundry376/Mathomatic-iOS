
                                   Mathomatic
                                       by
                               George Gesslein II

This archive contains the C source code and documentation for Mathomatic, the
automatic algebraic manipulator.  Using the included makefiles, this console
mode program should compile with gcc and run correctly under Unix, Linux, Mac
OS X, and Cygwin, without any modifications.  Most of the Mathomatic code can
also be called from any C or C++ program, when linked with the static library
created by "makefile.lib".

Mathomatic is a free, portable, general-purpose CAS (Computer Algebra System)
and calculator software that can symbolically solve, simplify, combine, and
compare equations, perform complex number and polynomial arithmetic, etc.  It
does some calculus and is very easy to use.

All software and documentation in this archive is copyrighted and made
available under the GNU Lesser General Public License (LGPL) version 2.1 (see
file "COPYING").  Mathomatic comes with no warranty at all, but if you find
any bugs, I will try to fix them.


                                  Compilation
                                  -----------

This section can be skipped; you may wish to jump to the next section with
short compile/install instructions, or read the file "INSTALL.txt", if you
only wish to install Mathomatic.

To compile under Mac OS X, Xcode Tools should be installed, which contains
the gcc C compiler and make utility.  Xcode Tools is available on one of the
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

This allows you to use the cursor keys to recall and edit previously entered
or pushed expressions when running Mathomatic.

To compile Mathomatic as a stand-alone executable that has no shared library
dependencies, type:

	LIBS=-static make

The resulting binary will run on any system with the same operating system
(like Linux) and architecture (like x86), because the libraries are static
(included with the binary).  Unfortunately, the "-static" option cannot be
combined with readline, due to the way the readline library works.

To compile the secure version, with no file I/O or shelling out, type:

	./compile.secure

This will silently create the executable "mathomatic_secure", which can
safely be used as a telnet application or CGI program.


                                  Installation
                                  ------------

A typical installation is done by typing the following at the shell prompt:

	make READLINE=1
	make test
	sudo make install

This will compile, test, and install the Mathomatic executable and docs in
"/usr/local" in less than a minute.

There are quite a few math goodies in this archive, besides the main
Mathomatic program:

  The directory "icons" contains Mathomatic icons for your desktop.
  The directory "doc" contains the Mathomatic documentation in HTML.
  The directory "tests" contains test scripts and other interesting scripts.
  The directory "primes" contains a prime number generator utility, etc.
  The directory "lib" contains the API and test for the Mathomatic library.
  The directory "m4" enables function entry using the GNU m4 front-end.
  The directory "misc" contains a polynomial equation solver utility.

For quick help while running Mathomatic, type "?" or use the help command.
To read or print the documentation, point your web browser to the file
"doc/index.html", or "/usr/local/share/doc/mathomatic/html/index.html" if
Mathomatic was installed.  When copying the Mathomatic documentation, please
copy the entire documentation directory, not just selected files from it.

For the latest source code, documentation, and information, go to the
Mathomatic website: http://mathomatic.org

Author contact information:
  email:
    georgegesslein@gmail.com
  postal address:
    George Gesslein II
    43 Atwater Road
    Lansing, New York 14882
    USA

Please don't be shy about reporting bugs or problems using Mathomatic, just
send emails to the author or report bugs on the web at
"http://bugs.launchpad.net/mathomatic".


             Compile-time defines for the Mathomatic source code
             ---------------------------------------------------

To compile Mathomatic for UNIX, GNU/Linux, Mac OS X, or any POSIX compliant
OS, define "UNIX" (see "makefile").  To compile Mathomatic for a generic
system, simply compile with no defines.  To compile for Microsoft Win32 using
Cygwin, define only "CYGWIN" (see "makefile.cygwin").  To compile for
hand-helds like the Nintendo DS, define "HANDHELD" and "SECURE" for greatly
reduced memory usage and no file I/O.

Define "READLINE" and include the readline libraries at link time to use
readline mode.  This will allow easy command line editing and history.

Define "SILENT" to remove all helpful messages and debugging code.  This is
useful when using Mathomatic as a symbolic math library.  Code size is
reduced with this option.

Define "LIBRARY" when using the Mathomatic code as a symbolic math library.
"SILENT" is automatically defined when this is defined.  Most standard input
and output is disabled.  See the directory "lib" and the file "makefile.lib"
for the library hooks (API) and test program.  The following commands are
omitted with this option: calculate, code, divide, edit, nintegrate,
optimize, pause, quit, roots, tally, and variables.

Define "SECURE" to disable all file reading, writing, and executing.  This is
useful when making Mathomatic available to the public through telnet or CGI
programs.  It is also useful when making ROMable code.  All insecure commands
are omitted with this option.  See "compile.secure", which is the secure
Mathomatic build script.

Define "TIMEOUT_SECONDS" to set the maximum number of seconds Mathomatic may
run.  Upon timeout, Mathomatic properly exits.  This is useful when making
Mathomatic a CGI program, so it won't overload the server.

Define "I18N" to enable internationalization.  Currently no translations have
been made, so Mathomatic is only available in English.


                         Mathomatic C source code files
                         ------------------------------

  am.h - the main include file for Mathomatic, contains tunable parameters
  complex.h - floating point complex number arithmetic function prototypes
  externs.h - global variable extern definitions
  proto.h - global function prototypes, can be made with cproto

  am.c - many necessary routines
  cmds.c - code for commands that don't belong anywhere else
  complex.c - floating point complex number routines for Mathomatic
  complex_lib.c - generic floating point complex number arithmetic library
  diff.c - differentiation routines and commands
  factor.c - symbolic factorizing routines (not polynomial factoring)
  factor_int.c - floating point constant factorizing routines
  gcd.c - floating point GCD and fractions code
  globals.c - global variable and array definitions
  help.c - command table, help command code, and input parsing routines
  integrate.c - integration routines and commands
  list.c - expression and equation display routines
  main.c - startup code for Mathomatic
  parse.c - mathematical expression parsing routines
  poly.c - simplifying and polynomial routines
  simplify.c - simplifying routines
  solve.c - symbolic solving routines
  super.c - group and combine denominators of fractions
  unfactor.c - unfactorizing (expanding) routines
