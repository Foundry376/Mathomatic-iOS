/*
 * Tested long integer square root functions.
 */

#include <stdio.h>

#define	true	1
#define	false	0

#if	VERIFY
/*
 * Return true if x is the truncated integer square root of y.
 */
int
verify_lsqrt(long y, long x)
{
	if ((long long) x * x > y || ((long long) x + 1) * ((long long) x + 1) <= y) {
		return false;
	}
	return true;
}
#endif

#if	false
/*
 * Returns the truncated integer square root of y using the Babylonian iterative approximation method.
 * Return -1 on error.
 */
long
lsqrt(long y)
{
	long	x_old, x_new;
	long	testy;
	int	nbits;
	int	i;

	if (y <= 0) {
		if (y != 0) {
			fprintf(stderr, "Domain error in %s().\n", __func__);
			return -1L;
		}
		return 0L;
	}
/* select a good starting value using binary logarithms: */
	nbits = (sizeof(y) * 8) - 1;	/* subtract 1 for sign bit */
	for (i = 4, testy = 16L;; i += 2, testy <<= 2L) {
		if (i >= nbits || y <= testy) {
			x_old = (1L << (i / 2L));	/* x_old = sqrt(testy) */
			break;
		}
	}
/* x_old >= sqrt(y) */
/* use the Babylonian method to arrive at the integer square root: */
	for (;;) {
		x_new = (y / x_old + x_old) / 2L;
		if (x_old <= x_new)
			break;
		x_old = x_new;
	}
	return x_old;
}
#else
/*
 * Faster and simpler binary digit algorithm for calculating the truncated integer square root.
 * Copied from Wikipedia and modified.
 *
 * Returns the truncated integer square root of num.
 * Return -1 on error.
 */
long
lsqrt(long num)
{
	long op = num;
	long result = 0;
	long one = 1L << (sizeof(num) * 8 - 2); // The second-to-top bit is set

	if (num <= 0) {
		if (num != 0) {
			fprintf(stderr, "Domain error in %s().\n", __func__);
			return -1L;
		}
		return 0L;
	}
// "one" starts at the highest power of four <= the argument.
	while (one > op)
		one >>= 2;

	while (one != 0) {
		if (op >= result + one) {
			op -= result + one;
			result += 2 * one;
		}
		result >>= 1;
		one >>= 2;
	}
#if	VERIFY
	if (!verify_lsqrt(num, result)) {
		fprintf(stderr, "Error in %s().\n", __func__);
		return -1L;
	}
#endif
	return result;
}
#endif
