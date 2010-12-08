/*
 * Generate batches of consecutive prime numbers using a modified sieve of Eratosthenes
 * algorithm that doesn't use much memory by using a windowing sieve buffer.
 *
 * Copyright (C) 2009 George Gesslein II.
 
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public 
    License as published by the Free Software Foundation; either 
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of 
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.
 
    You should have received a copy of the GNU Lesser General Public 
    License along with this library; if not, write to the Free Software 
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
 */

/*
 * Changes:
 *
 * 11/22/05 - converted everything to long doubles.  Now uses C99 long double functions.
 * 3/25/06 - made primes buffer variable size.
 * 3/30/08 - Allow long double to be aliased to double when long double isn't supported.
 * 2/11/09 - Cleanup calculation of number of decimal digits and max_integer.
 * 9/12/10 - General cleanup and added error message for when the requested number of primes to display is not reached.
 * 10/25/10 - Added -c and -h options.
 * 10/26/10 - Using usage2() instead of usage() most of the time, now.
 * 10/28/10 - Fixed to work with 16-bit integers.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <math.h>
#include <float.h>
#include <assert.h>

#define	true	1
#define	false	0

#ifndef	max
#define max(a, b)    (((a) > (b)) ? (a) : (b))		/* return the maximum of two values */
#endif

#ifndef	min
#define min(a, b)    (((a) < (b)) ? (a) : (b))		/* return the minimum of two values */
#endif

#define	ARR_CNT(a)	((int) (sizeof(a)/sizeof(a[0])))	/* returns the number of elements in an array */

/* Maximum memory usage in bytes; can be set to any size. */
#define BUFFER_SIZE	2000000
#if	BUFFER_SIZE >= (INT_MAX / 2) || BUFFER_SIZE <= 0
#warning BUFFER_SIZE out of range, using INT_MAX / 2.
#undef BUFFER_SIZE
#define BUFFER_SIZE	min((INT_MAX / 2), 2000000)
#endif

void		generate_primes(void);
int		test_pal(long double d, long double base);
void		usage(int ev);
void		usage2(int ev);
int		get_long_double_int(char *cp, long double *dp);
long double	powl(), ceill(), sqrtl(), fmodl(), strtold();

long double max_integer;		/* largest value of a long double integral value */
long double start_value = -1.0;		/* where to start finding primes */
long double number;			/* number of prime lines to display */
int count_requested = false;		/* true if the number of primes to display is set by "number" above */
long double default_number = 20;	/* default number of primes to display */
long double end_value;			/* where to stop finding primes */
long double skip_multiples[] = {	/* Additive array that skips over multiples of 2, 3, 5, and 7. */
	10, 2, 4, 2, 4, 6, 2, 6,
	 4, 2, 4, 6, 6, 2, 6, 4,
	 2, 6, 4, 6, 8, 4, 2, 4,
	 2, 4, 8, 6, 4, 6, 2, 4,
	 6, 2, 6, 6, 4, 2, 4, 6,
	 2, 6, 4, 2, 4, 2,10, 2
};	/* sum of all numbers = 210 = (2*3*5*7) */

int		pal_flag, twin_flag;
long double	pal_base = 10;	/* The palindrome base, if displaying palindromic primes. */

char		*prime;		/* The boolean sieve array (buffer) for the current window of numbers being tested for primality; */
				/* each char (byte) contains true or false, true if prime. */
int		buffer_size;	/* Number of bytes for variable size prime[] buffer above. */

char		*prog_name = "matho-primes";

int
main(int argc, char *argv[])
{
	extern char	*optarg;	/* set by getopt(3) */
	extern int	optind;

	int		i;
	char		buf[1000];

#if	DEBUG
	fprintf(stderr, "BUFFER_SIZE = %d\n", BUFFER_SIZE);
#endif
/* set the highest number this program will work with: */
	max_integer = powl(10.0L, (long double) (LDBL_DIG));
	while (max_integer == max_integer + 1.0) {
		fprintf(stderr, "Warning: max_integer (%Lg) is too large; size of long double = %u bytes.\n", max_integer, (unsigned) sizeof(long double));
		max_integer /= 10.0L;
	}
	end_value = max_integer;
	number = 0;
/* process command line options: */
	while ((i = getopt(argc, argv, "c:thp:")) >= 0) {
		switch (i) {
		case 'c':
			count_requested = true;
			if (optarg && !get_long_double_int(optarg, &number)) {
				usage2(2);
			}
			break;
		case 'h':
			usage2(0);
			break;
		case 't':
			twin_flag = true;
			break;
		case 'p':
			pal_flag = true;
			if (optarg && !get_long_double_int(optarg, &pal_base)) {
				usage2(2);
			}
			break;
		default:
			usage2(2);
		}
	}
	if (argc > optind) {
		if (strncmp(argv[optind], "twin", 4) == 0) {
			twin_flag = true;
			optind++;
		}
	}
	if (argc > optind && isdigit(argv[optind][0])) {
		if (get_long_double_int(argv[optind], &start_value)) {
			optind++;
		} else {
			usage2(2);
		}
		if (argc > optind && isdigit(argv[optind][0])) {
			if (get_long_double_int(argv[optind], &end_value)) {
				if (end_value < start_value) {
					fprintf(stderr, "End value is less than start value.\n");
					usage2(2);
				}
				optind++;
				if (number == 0)
					number = max_integer;
			} else {
				usage2(2);
			}
		}
	}
	if (argc > optind) {
		if (strncmp(argv[optind], "twin", 4) == 0) {
			twin_flag = true;
			optind++;
		}
	}
	if (argc > optind) {
		if (strncmp(argv[optind], "pal", 3) == 0) {
			pal_flag = true;
			optind++;
		} else {
			usage(2);
		}
		if (argc > optind) {
			if (!get_long_double_int(argv[optind], &pal_base)) {
				usage(2);
			}
			optind++;
		}
	}
	if (argc > optind) {
		if (strncmp(argv[optind], "twin", 4) == 0) {
			twin_flag = true;
			optind++;
		}
	}
	if (argc > optind) {
		fprintf(stderr, "Unrecognized argument.\n");
		usage(2);
	}
	if (pal_base < 2 || pal_base >= INT_MAX) {
		fprintf(stderr, "Palindrome number base must be >= 2.\n");
		usage(2);
	}
	if (start_value < 0.0) {
get1:
		fprintf(stderr, "Enter number to start finding primes at (0): ");
		if (fgets(buf, sizeof(buf), stdin) == NULL)
			exit(1);
		switch (buf[0]) {
		case '\0':
		case '\n':
		case '\r':
			start_value = 0;
			break;
		default:
			if (!get_long_double_int(buf, &start_value)) {
				goto get1;
			}
		}
	}
	if (number == 0) {
get2:
		if (twin_flag) {
			fprintf(stderr, "Enter number of prime twins to output (%Lg): ", default_number);
		} else {
			fprintf(stderr, "Enter number of primes to output (%Lg): ", default_number);
		}
		if (fgets(buf, sizeof(buf), stdin) == NULL)
			exit(1);
		switch (buf[0]) {
		case '\0':
		case '\n':
		case '\r':
			number = default_number;
			break;
		default:
			if (!get_long_double_int(buf, &number)) {
				goto get2;
			}
		}
		count_requested = true;
	}
/* allocate the prime[] buffer: */
	buffer_size = (int) min(BUFFER_SIZE, (end_value - start_value) + 1.0L);
#if	DEBUG
	fprintf(stderr, "buffer_size = %d\n", buffer_size);
#endif
	prime = (char *) malloc(buffer_size);
	if (prime == NULL) {
		fprintf(stderr, "%s: Not enough memory for buffer_size = %d.\n", prog_name, buffer_size);
		exit(2);
	}
	generate_primes();
	exit(0);
}

/*
 * Eliminate all multiples of "arg" from the sieve array by setting their entries to false.
 */
void
elim_factor(long double arg)
{
	long double	d;
	int		i, j;

	d = ceill(start_value / arg);
	if (d < 2.0)
		d = 2.0;
	d *= arg;
	d -= start_value;
	if (d >= buffer_size)
		return;
	i = (int) d;
	if (i >= buffer_size)
		return; 
	assert(i >= 0);
	if (arg >= buffer_size) {
		prime[i] = false;
	} else {
		j = (int) arg;
		assert(j > 0);
		for (; i < buffer_size; i += j) {
			prime[i] = false;
		}
	}
}

/*
 * Generate and display at most "number" consecutive prime numbers,
 * between "start_value" and "end_value".
 */
void
generate_primes(void)
{
	int		n, j;
	long double	count, ii, vv, last_prime = -3.0;

	for (count = 0; count < number; start_value += buffer_size) {
		if (start_value > end_value) {
			goto check_return;
		}
		/* generate a batch of consecutive primes with the prime number sieve */
		memset(prime, true, buffer_size);	/* set the prime array to all true */
		vv = 1.0 + sqrtl(min(start_value + (long double) buffer_size, end_value));
		elim_factor(2.0L);
		elim_factor(3.0L);
		elim_factor(5.0L);
		elim_factor(7.0L);
		for (ii = 1.0; ii <= vv;) {
			for (j = 0; j < ARR_CNT(skip_multiples); j++) {
				ii += skip_multiples[j];
				elim_factor(ii);
			}
		}
		/* display the requested part of the batch of generated prime numbers */
		for (n = 0; n < buffer_size && count < number; n++) {
			if (prime[n]) {	/* if prime number */
				ii = start_value + n;
				if (ii > end_value) {
					goto check_return;
				}
				if (ii > 1.0) {
					if (pal_flag && !test_pal(ii, pal_base))
						continue;
					if (twin_flag) {
						if ((last_prime + 2.0L) == ii) {
							printf("%.0Lf %.0Lf\n", last_prime, ii);
							count++;
						}
					} else {
						printf("%.0Lf\n", ii);
						count++;
					}
					last_prime = ii;
				}
			}
		}
	}
check_return:
	if (count_requested && count < number) {
		fprintf(stderr, "%s: Number of primes requested not reached.\n", prog_name);
		exit(1);
	}
}

/*
 * Parse a space or null terminated ASCII number in the string pointed to by "cp".
 *
 * Return true with a floating point long double value in "*dp" if a valid integer,
 * otherwise display an error message and return false.
 */
int
get_long_double_int(char *cp, long double *dp)
{
	char	*cp1;

	errno = 0;
	*dp = strtold(cp, &cp1);
	if (errno) {
		perror(NULL);
		return false;
	}
	if (cp == cp1) {
		fprintf(stderr, "Invalid number.\n");
		return false;
	}
	switch (*cp1) {
	case '\0':
	case '\r':
	case '\n':
		break;
	default:
		if (isspace(*cp1)) {
			break;
		}
		fprintf(stderr, "Invalid number.\n");
		return false;
	}
	if (*dp > max_integer) {
		fprintf(stderr, "Number is too large, maximum is %Lg.\n", max_integer);
		return false;
	}
	if (*dp < 0.0 || fmodl(*dp, 1.0L) != 0.0) {
		fprintf(stderr, "Number must be a positive integer or zero.\n");
		return false;
	}
	return true;
}

/*
 * Return true if "d" is a palindromic number, base "base".
 */
int
test_pal(long double d, long double base)
{
#define	MAX_DIGITS	1000

	int		i, j;
	int		digits[MAX_DIGITS];

	/* build the array of digits[] */
	for (i = 0; d >= 1.0; i++) {
		assert(i < MAX_DIGITS);
		digits[i] = (int) (fmodl(d, base));
		d /= base;
	}
	/* compare the array of digits[] end to end */
	for (j = 0, i--; j < i; j++, i--) {
		if (digits[i] != digits[j])
			return false;
	}
	return true;
}

void
usage(int ev)
{
	printf("\nPrime number generator version 1.1\n");
	printf("Usage: %s [start [stop]] [\"twin\"] [\"pal\" [base]]\n\n", prog_name);
	printf("Generate consecutive prime numbers from start to stop up to %Lg.\n", max_integer);
	printf("If \"twin\" is specified, output only twin primes.\n");
	printf("If \"pal\" is specified, output only palindromic primes.\n");
	printf("The palindrome number base may be specified, the default is base 10.\n");
	exit(ev);
}

void
usage2(int ev)
{
	printf("\nPrime number generator version 1.1\n");
	printf("Usage: %s [options] [start [stop]]\n\n", prog_name);
	printf("Generate consecutive prime numbers from start to stop up to %Lg.\n", max_integer);
	printf("Options:\n");
	printf("  -c count         Count lines of primes, stop when count reached.\n");
	printf("  -h               Display this help and exit.\n");
	printf("  -p base          Output only palindromic primes.\n");
	printf("  -t               Output only twin primes.\n");
	exit(ev);
}
