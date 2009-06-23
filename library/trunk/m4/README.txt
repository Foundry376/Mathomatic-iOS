                                m4 Mathomatic
                                -------------

Type "./rmath" from the shell prompt to run Mathomatic using GNU m4 as a
macro pre-processor for allowing easy entry of math functions.  The
executable file "rmath" is a shell script and is only known to work with GNU
software.  The "matho" and "rmath" shell scripts can be installed for
system-wide use with "make m4install".  "rmath" stands for readline
Mathomatic and runs "matho" with a readline wrapper.  "matho" runs m4 and
pipes the output into Mathomatic.

Available macros for functions and constants are listed in the file
"functions.m4".  Trigonometric functions (sin(x), cos(x), etc.) are
implemented as complex exponentials in Mathomatic.

Input is filtered by m4, so opening a shell or an editor doesn't work.

The "quit" and "exit" commands may have some delay.  You can use the EOF
character (control-D) to quit instantly, instead.

The read command currently doesn't use m4, so it can't process functions.
The way to read in text files with functions is to supply the filenames on
the command line:

	rmath filenames
