#include <math.h>

/*
 * General factorial function in C for double precision floating point.
 * Uses the lgamma_r(3) function, like Mathomatic does,
 * so it works for any floating point value.
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
