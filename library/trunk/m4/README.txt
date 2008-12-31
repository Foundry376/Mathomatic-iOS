Type "./math" from the shell prompt to run Mathomatic using GNU m4 as a macro
pre-processor for allowing easy entry of math functions.  The executable file
"math" is a shell script and is only known to work with GNU software.  The
"math" and "rmath" shell scripts can be installed for system-wide use with
"make m4install".

Available macros for functions and constants are listed in the file
"functions.m4".  Trigonometric functions (sin(x), cos(x), etc.) are
implemented as complex exponentials in Mathomatic.

Input is filtered by m4, so opening a shell or an editor doesn't work.

Readline doesn't work while using m4, unless rlwrap (readline wrapper) is
also used as a front end.  The rlwrap program may be obtained at
http://utopia.knoware.nl/~hlub/uck/rlwrap/ and is invoked by "./rmath".

The "quit" and "exit" commands have some delay.  You can use the EOF
character (control-D) to quit, instead.

The read command currently doesn't use m4, so it can't process functions.
The way to read in text files with functions is to supply the filenames on
the command line:

	math filenames
