
                       Mathomatic Symbolic Math Library
                       --------------------------------

This directory contains the API (Application Programming Interface) "lib.c"
and test/example program "testmain.c" for the Mathomatic symbolic math
library. The API can be used to link your C compatible programs with the
Mathomatic symbolic math engine. This simple API provides for passing C text
strings containing expressions and commands to the Mathomatic engine. If
successful, a text string containing the resulting expression is returned,
otherwise an error message is returned.

Mathomatic is released under the GNU Lesser General Public License, so that
even closed-source software can make use of it.

This symbolic math library is at least able to be run anywhere the main
Mathomatic application can be run, and does not require an operating system
beyond the ability to allocate memory with malloc(3). The symbolic math
library is not re-entrant, meaning it cannot successfully be called again
until the last call to it completes. This is due to the fact that most data
storage areas in Mathomatic are global and static. I think that also means
Mathomatic is not threadsafe, correct me if I am wrong.

To compile this Mathomatic symbolic math library and its test program, type
"make" while in this "lib" directory. This will create the static library
"libmathomatic.a" and the API test executable named "testmain". To run the
test executable, type "./testmain".

To do a system install of the development library "libmathomatic.a" and C
header file "mathomatic.h", type:

	make
	sudo make install

Just include the file "mathomatic.h" and call the functions in "lib.c" to use
this library. Link your program with "libmathomatic.a" by using "-lmathomatic
-lm" at the end of the ld linker command line. "libmathomatic.a" is not
required on the target system, since it is a static library, meaning it is
included in the resulting executable as needed.

The following code provides a quick test of this library:

    char *output;
    matho_init();
    matho_process("x^2=4", &output);
    matho_process("solve x", &output);
    printf("%s\n", output);

Remember to free(output) on each successful call to matho_process() and
matho_parse() to return the memory used by the output string when you are
finished using it, otherwise there will be a memory leak. Here is the above
code fixed properly so it doesn't leak:

    char *output;
    int rv;
    if (!matho_init()) {
        printf("Not enough memory.\n");
        exit(1);
    }
    matho_process("x^2=4", NULL);
    rv = matho_process("solve x", &output);
    if (output) {
        printf("%s\n", output);
        if (rv) {
            free(output);
        }
    }

The result of running the above code when linked with the Mathomatic library
should be:

	x = 2*sign

The following Mathomatic commands are omitted in this library: calculate,
edit, plot, push, quit, and tally. To make up for the lack of the calculate
command, the replace, approximate, and "simplify sign" commands are provided,
allowing very similar functionality.

Recently, the "nintegrate" and "integrate definite" commands are allowed; the
bounds of integration are specified in the two equation spaces following the
current equation. cur_equation+1 is the lower bound, and cur_equation+2 is
the upper bound. Of course, the integrate and nintegrate commands operate on
the current equation, specified by the global "cur_equation" variable (origin
0). The equation number displayed is always origin 1, making it 1 greater
than "cur_equation", so keep that in mind.

Please define the C preprocessor name HANDHELD=1 when compiling this library
for handheld computing devices like the iPhone and other small computing
devices, for reduced memory usage. For embedded devices with no file storage,
or for privacy or no wish to use file storage, define SECURE=1 too.

Please read the file "../README.txt" for more developer information. These
files were written by George Gesslein II of www.mathomatic.org
