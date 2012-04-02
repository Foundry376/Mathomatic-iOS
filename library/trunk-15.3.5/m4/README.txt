
                                m4 Mathomatic
                                -------------

The executable named "mathomatic" is the best to run, however if you would
like functions, you need to run "rmath", which runs Mathomatic using GNU m4
as a macro pre-processor, allowing easy entry of standard math functions like
sqrt() and sin(). "rmath" and "matho" are shell scripts and are only known to
work with GNU software. "rmath" runs "matho" with a readline wrapper, if
available. "matho" runs m4 and pipes the output into Mathomatic.

To permanently install these program files on your system if this is a binary
distribution, type:

	sudo ./matho-install

To undo the above command:

	sudo ./matho-uninstall

Installation is not necessary to run Mathomatic. "sudo make m4install" is the
proper way to install these files from the source distribution.

Defined macros for functions and constants are listed in the file
"functions.m4". Trigonometric functions (sin(), tan(), etc.) are implemented
as complex exponentials in Mathomatic:

 1-> sin(x)^2+cos(x)^2

      ((e#^(i#*x)) - (e#^(-1*i#*x)))       ((e#^(i#*x)) + (e#^(-1*i#*x)))
 #1: (------------------------------^2) + (------------------------------^2)
                  (2*i#)                                 2

 1-> simplify

 #1: 1

 1-> 

Mathomatic input is filtered by m4, so opening a shell or an editor doesn't
work when running m4 Mathomatic.

The "quit" and "exit" commands may have some delay. You can use the EOF
character (control-D) to quit instantly, instead.

The read command currently doesn't use m4, so it can't process functions. The
way to read in text files with functions is to supply the filenames on the
shell command line:

	rmath filenames

All Mathomatic functions are real number, complex number, and symbolically
capable.

----------------------------------------------------------------------------

You can turn on color mode with the Mathomatic command:

	set color

or

	set bold color

typed at the Mathomatic main prompt. Then save the current color state with:

	set save

so that Mathomatic starts up with colors every time. To turn off color mode,
type:

	set no color
