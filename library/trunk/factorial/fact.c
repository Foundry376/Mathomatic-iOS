#include <math.h>

/*
 * General factorial function in C for double precision floating point.
 * Uses the threadsafe lgamma_r(3) function.
 * Works for any floating point value.
 * Recommend using tgamma(3) (true gamma) function instead, if available.
 *
 * Link with -lm
 *
 * Returns (arg!) (same as gamma(arg+1)).
 * Sets "errno" external variable on overflow.
 */
double
fact(double arg)
{
	int	sign;
	double	d;

	d = exp(lgamma_r(arg + 1.0, &sign));
	return(d * sign);
}
