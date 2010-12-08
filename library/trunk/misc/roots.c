/*
 * Command line numerical polynomial equation solver using the GNU Scientific Library (GSL).
 * GSL is available from: "http://www.gnu.org/software/gsl/".
 *
 * Copyright (C) 2008 George Gesslein II
 
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
 * To compile:

./c

 * or

cc -O -Wall -o roots roots.c -lgsl -lgslcblas

 * This program nicely solves any degree polynomial
 * with real coefficients by calling the
 * GSL function gsl_poly_complex_solve().
 * This file is also useful for testing
 * and as an example of using the GSL from C.
 * The results are double precision floating point values
 * that are usually accurate to 14 digits.
 * Solutions are determined by the GSL using
 * balanced-QR reduction of the companion matrix.
 * Complex numbers are output if successful.
 * Here is an example of it solving a cubic polynomial:

[~/c/am/misc] roots 1 1 1 1
The 3 approximate floating point solutions of:
+1*x^3 +1*x^2 +1*x +1 = 0
are:

x = +0 +1*i
x = +0 -1*i
x = -1
[~/c/am/misc] 

 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <float.h>
#include <math.h>
#include <errno.h>
#include <gsl/gsl_poly.h>
#include <gsl/gsl_errno.h>

#define EPSILON	0.00000000000005	/* a good value for doubles */
#define HELP	1			/* display helpful text */

void	display_root(int i);

char	*prog_name = "roots";		/* name of this program */
double	*a, *z;		/* input and output arrays, respectively */

void
usage()
{
  printf("\n%s version 1.0 - numerical polynomial equation solver\n", prog_name);
  printf("Uses the GNU Scientific Library (GSL).\n");
  printf("\nSolves polynomial = 0 when given all real coefficients of the polynomial.\n");
  printf("Double precision floating point math is used, accurate to about 14 digits.\n");
  printf("\nUsage: %s highest-power-coefficient ... constant-term\n", prog_name);
  printf("\nThe coefficients must be decimal, floating point numbers.\n");
}

int
main(int argc, char **argv)
{
  int i, highest_power;
  char *cp, *arg;
  gsl_poly_complex_workspace *w;

  if (argc <= 1) {
    fprintf(stderr, "%s: The polynomial coefficients must be specified on the command line.\n", prog_name);
    usage();
    exit(2);
  }

  highest_power = argc - 2;
  a = calloc(highest_power + 1, sizeof(double)); /* allocate real double input array */
  z = calloc(2 * highest_power, sizeof(double)); /* allocate complex double output array */

/* parse the command line into the coefficient array a[] */
  for (i = 0; i < argc - 1; i++) {
    arg = argv[argc-i-1];
    errno = 0;
    a[i] = strtod(arg, &cp);
    if (arg == cp || *cp) {
      fprintf(stderr, "%s: Argument \"%s\" is not a floating point number.\n", prog_name, arg);
      usage();
      exit(2);
    }
    if (errno) {
      fprintf(stderr, "%s: Argument \"%s\" is out of range.\n", prog_name, arg);
      exit(2);
    }
  }

#if	HELP
/* nicely display the actual polynomial equation we are solving */
  printf("The %d approximate floating point solutions of:\n", highest_power);
  for (i = highest_power; i >= 0; i--) {
    if (a[i]) {
      printf("%+.*g", DBL_DIG, a[i]);
      if (i) {
	printf("*x");
      }
      if (i > 1) {
        printf("^%d", i);
      }
      printf(" ");
    }
  }
  printf("= 0\nare:\n\n");
#endif

/* solve the polynomial equation */
  w = gsl_poly_complex_workspace_alloc(highest_power + 1);
  if (gsl_poly_complex_solve(a, highest_power + 1, w, z) != GSL_SUCCESS) {
    fprintf(stderr, "%s: GSL approximation failed.\n", prog_name);
    exit(1);
  }
  gsl_poly_complex_workspace_free(w);

/* display all solutions */
  for (i = 0; i < highest_power; i++) {
#ifdef	EPSILON /* zero out relatively very small values (which are floating point error) */
    if (fabs(z[2*i] * EPSILON) > fabs(z[2*i+1]))
      z[2*i+1] = 0.0;
    else if (fabs(z[2*i+1] * EPSILON) > fabs(z[2*i]))
      z[2*i] = 0.0;
#endif
#if	HELP
    printf("x = ");
#endif
    display_root(i);
    printf("\n");
  }

  exit(0);
}

void
display_root(int i)
{
  printf("%+.*g", DBL_DIG, z[2*i]);		/* output real part */
  if (z[2*i+1])
    printf(" %+.*g*i", DBL_DIG, z[2*i+1]);	/* output imaginary part */
}
