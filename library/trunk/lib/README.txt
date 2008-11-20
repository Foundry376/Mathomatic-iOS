This directory contains the API and test for the Mathomatic symbolic math
library, which can be used to link your C and C++ programs with the
Mathomatic symbolic math engine.  Compile and link "*.c" and "../*.c" with
"-DLIBRARY" on the C compiler command line, to create the test application
which uses this API.  This is automatically done by "../makefile.lib".  To
compile the library and test program, go to the top of the Mathomatic source
tree and type:

	make clean
	make -f makefile.lib

This will create the API test executable named "mathomatic" and the static
library "libmathomatic.a".  To run the test executable, type "./mathomatic".
To do a system install of the library and C header file "mathomatic.h", type:

	sudo make -f makefile.lib install

See the file "lib.c" for the simple API, and "test.c" for a simple example of
how to use it.  Just include the file "mathomatic.h" and call the functions
in "lib.c" to use this library.  Link your program with "libmathomatic.a" by
using "-lmathomatic -lm" at the end of the ld linker command line.
