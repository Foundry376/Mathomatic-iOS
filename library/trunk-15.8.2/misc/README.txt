
Useful stuff that doesn't belong anywhere else goes in this directory. These
files do not have any man page, because I think they are not ready or not
made for packaging.  Also included are a few imperfect test scripts (*.in).

ideas.txt - The Mathomatic todo and idea list.

known_bugs.txt - Current list of known Mathomatic bugs.
                 Normally bugs are fixed immediately.

limits.c - Small C program to display current C data type limits and sizes.

roots.c - Nice GSL example of a numerical polynomial equation solver utility.
          Compile with "./c", requires the libgsl development files.

c - Shell script to compile "roots.c".

testprimes - A parallel, brute force test of the prime number generator.
             Checks the first 50,000,000 primes for gaps or errors.
