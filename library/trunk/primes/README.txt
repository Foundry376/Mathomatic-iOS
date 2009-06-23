                        Mathomatic Prime Number Tools
                        -----------------------------

This directory contains some small, integer math programs written in C.  To
compile and install, type the following commands at the Unix shell prompt:

	make
	make test
	sudo make install

This will install:

	matho-pascal - display Pascal's triangle
	matho-primes - quickly generate many consecutive prime numbers
	matho-sumsq - display minimum sum of the squares

The Python program "primorial" is included in this directory for calculating
large primorials from "matho-primes".  To generate a list of all unique
primorials from 2 to 97, type the following at the Unix shell:

	./primorial `matho-primes 2 97`

These utilities are part of Mathomatic.  See www.mathomatic.org to get the
latest version.
