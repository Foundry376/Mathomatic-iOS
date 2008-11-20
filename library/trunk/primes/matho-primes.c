/*
 * Generate batches of consecutive prime numbers using a modified sieve of Eratosthenes
 * algorithm that doesn't use much memory by using a windowing sieve buffer.
 *
 * Copyright (C) 2007 George Gesslein II.
 */

/*
Usage: matho-primes [start [stop]] ["twin"] ["pal" [base]]

Generate batches of consecutive prime numbers up to 19 digits.
If "twin" is specified, output only twin primes.
If "pal" is specified, output only palindromic primes.
The palindromic base may be specified, the default is base 10.

or

Usage: matho-primes [options] [start [stop]]

Generate batches of consecutive prime numbers up to 19 digits.
Options:
  -t               Output only twin primes.
  -p base          Output only palindromic primes.
*/

/*
 * Changes:
 *
 * 11/22/05 - converted everything to long doubles.  Now uses C99 long double functions.
 * 3/25/06 - made primes buffer variable size.
 * 3/30/08 - Allow long double to be aliased to double when long double isn't supported.
 */

#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <math.h>
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

/* Maximum memory usage in bytes; can be set to any size; the larger, the faster: */
#define BUFFER_SIZE	2000000
#if	BUFFER_SIZE >= (INT_MAX / 2) || BUFFER_SIZE <= 0
#error	"BUFFER_SIZE too large."
#endif

void		generate_primes(void);
int		test_pal(long double d, long double base);
void		usage(void);
void		usage2(void);
int		get_long_double_int(char *cp, long double *dp);
long double	ceill(), sqrtl(), fmodl(), strtold();

long double max_integer;		/* largest value of a long double integral value */
long double start_value;		/* where to start finding primes */
long double number;			/* number of primes to display */
long double default_number = 30;	/* default number of primes to display */
long double end_value;			/* where to stop finding primes */
long double skip_multiples[] = {	/* Additive array that skips over multiples of 2, 3, 5, and 7. */
	10, 2, 4, 2, 4, 6, 2, 6,
	 4, 2, 4, 6, 6, 2, 6, 4,
	 2, 6, 4, 6, 8, 4, 2, 4,
	 2, 4, 8, 6, 4, 6, 2, 4,
	 6, 2, 6, 6, 4, 2, 4, 6,
	 2, 6, 4, 2, 4, 2,10, 2
};	/* sum of all numbers = 210 = (2*3*5*7) */

long double	last_prime = -3.0;

int		pal_flag, twin_flag;
long double	pal_base = 10;

char		*prime;				/* the boolean sieve array for the current window of numbers */
int		buffer_size;			/* for variable size prime[] buffer */

char		*prog_name = "matho-primes";

int
main(int argc, char *argv[])
{
	extern char	*optarg;	/* set by getopt(3) */
	extern int	optind;

	int		i;
	char		buf[1000];

        prog_name = strdup(basename(argv[0]));
	start_value = -1.0;
/* set the highest number this program will work with: */
	if (sizeof(long double) >= 12) {
		max_integer = 1.0e19L;	/* 19 digits for long doubles */
	} else if (sizeof(long double) == 8) {
		max_integer = 1.0e14L;	/* 14 digits for doubles */
	} else {
		fprintf(stderr, "%s: Unsupported size of long double = %u bytes.\n", prog_name, sizeof(long double));
		exit(2);
	}
	end_value = max_integer;
	if (end_value == end_value + 1.0) {
		fprintf(stderr, "Warning: max_integer is too large; size of long double = %u bytes.\n", sizeof(long double));
	}
	number = 0;
/* process command line options: */
	while ((i = getopt(argc, argv, "tp:")) >= 0) {
		switch (i) {
		case 't':
			twin_flag = true;
			break;
		case 'p':
			pal_flag = true;
			if (optarg && !get_long_double_int(optarg, &pal_base)) {
				usage2();
			}
			break;
		default:
			usage2();
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
			usage();
		}
		if (argc > optind && isdigit(argv[optind][0])) {
			if (get_long_double_int(argv[optind], &end_value)) {
				if (end_value < start_value) {
					fprintf(stderr, "End value is less than start value.\n");
					usage();
				}
				optind++;
				number = max_integer;
			} else {
				usage();
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
			usage();
		}
		if (argc > optind) {
			if (!get_long_double_int(argv[optind], &pal_base)) {
				usage();
			}
			optind++;
		}
	}
	if (argc > optind) {
		usage();
	}
	if (pal_base < 2 || pal_base >= INT_MAX) {
		fprintf(stderr, "Palindromic base must be >= 2.\n");
		usage();
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
		fprintf(stderr, "Enter number of primes to output (%Lg): ", default_number);
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
	}
/* allocate the prime[] buffer: */
	buffer_size = (int) min(BUFFER_SIZE, (end_value - start_value) + 1.0L);
	prime = (char *) malloc(buffer_size);
	if (prime == NULL) {
		fprintf(stderr, "%s: Not enough memory.\n", prog_name);
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
	assert(i >= 0);
	if (arg >= buffer_size) {
		prime[i] = false;
	} else {
		j = (int) arg;
		for (; i < buffer_size; i += j) {
			prime[i] = false;
		}
	}
}

/*
 * Generate and display consecutive prime numbers.
 */
void
generate_primes(void)
{
	int		n, j;
	long double	count, ii, vv;

	for (count = 0; count < number; start_value += buffer_size) {
		if (start_value > end_value) {
			return;
		}
		/* generate a batch of consecutive primes with the prime number sieve */
		memset(prime, true, buffer_size);	/* set the prime array to all true */
		vv = 1.0 + sqrtl(min(start_value + (long double) buffer_size, end_value));
		elim_factor(2.0L);
		elim_factor(3.0L);
		elim_factor(5.0L);
		elim_factor(7.0L);
		ii = 1.0;
		for (;;) {
			for (j = 0; j < ARR_CNT(skip_multiples); j++) {
				ii += skip_multiples[j];
				elim_factor(ii);
			}
			if (ii > vv)
				break;
		}
		/* display the requested part of the batch of generated prime numbers */
		for (n = 0; n < buffer_size && count < number; n++) {
			if (prime[n]) {
				ii = start_value + n;
				if (ii > end_value) {
					return;
				}
				if (ii > 1.0) {
					if (pal_flag && !test_pal(ii, pal_base))
						continue;
					if (twin_flag) {
						if ((last_prime + 2.0L) == ii) {
							printf("%.0Lf %.0Lf\n", last_prime, ii);
							count += 2;
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
}

/*
 * Parse a space or null terminated ASCII number in the string pointed to by "cp" and
 * return true with a floating point long double value in "*dp" if a valid integer,
 * otherwise display an error message and return false.
 */
int
get_long_double_int(char *cp, long double *dp)
{
	char	*cp1;

	*dp = strtold(cp, &cp1);
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
		fprintf(stderr, "Number is too large.\n");
		return false;
	}
	if (*dp < 0.0 || fmodl(*dp, 1.0L) != 0.0) {
		fprintf(stderr, "Number must be a positive integer.\n");
		return false;
	}
	return true;
}

/*
 * Return true if "d" is a palindrome base "base".
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
		digits[i] = (int) fmodl(d, base);
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
usage(void)
{
	printf("Prime number generator version 1.0\n");
	printf("Usage: %s [start [stop]] [\"twin\"] [\"pal\" [base]]\n\n", prog_name);
	printf("Generate batches of consecutive prime numbers up to %Lg.\n", max_integer);
	printf("If \"twin\" is specified, output only twin primes.\n");
	printf("If \"pal\" is specified, output only palindromic primes.\n");
	printf("The palindromic base may be specified, the default is base 10.\n");
	exit(2);
}

void
usage2(void)
{
	printf("Prime number generator version 1.0\n");
	printf("Usage: %s [options] [start [stop]]\n\n", prog_name);
	printf("Generate batches of consecutive prime numbers up to %Lg.\n", max_integer);
	printf("Options:\n");
	printf("  -t               Output only twin primes.\n");
	printf("  -p base          Output only palindromic primes.\n");
	exit(2);
}
