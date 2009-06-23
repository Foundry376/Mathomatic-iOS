                       Mathomatic Symbolic Math Library
                       --------------------------------

This directory contains the API (Application Program Interface) and test for
the Mathomatic symbolic math library, which can be used to link your C and
C++ programs with the Mathomatic symbolic math engine.  Compile and link
"*.c" and "../*.c" with "-DLIBRARY" on the C compiler command line, to create
the test application which uses this API.  This is automatically done by
"../makefile.lib".  To compile the library and test program, go to the top of
the Mathomatic source tree and type:

	make clean
	make -f makefile.lib

This will create the API test executable named "mathomatic" and the static
library "libmathomatic.a".  To run the test executable, type "./mathomatic".
To do a system install of the library and C header file "mathomatic.h", type:

	sudo make -f makefile.lib install

Be sure to "make clean" again, before using a different makefile.  See the
file "lib.c" for the simple API, and "test.c" for a simple example of how to
use it.  Just include the file "mathomatic.h" and call the functions in
"lib.c" to use this library.  Link your program with "libmathomatic.a" by
using "-lmathomatic -lm" at the end of the ld linker command line.

The following commands are omitted in the library: calculate, edit, optimize,
pause, quit, and tally.

Recently, the "nintegrate" and "integrate definite" commands are allowed, the
bounds are specified in the two equation spaces following the current
equation.  cur_equation+1 is the lower bound, and cur_equation+2 is the upper
bound.  Of course, the integrate and nintegrate commands operate on the
current equation, specified by the global cur_equation variable (origin 0).
The equation number displayed is always origin 1, making it 1 greater than
cur_equation, so keep that in mind.

Many bugs and inconsistencies in the symbolic math library have recently been
fixed, and it is best to always use a recent version of Mathomatic, when used
as a library.  The most recent version can always be found on the Mathomatic
website http://mathomatic.org
