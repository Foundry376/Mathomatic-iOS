/*
 * Command line numerical polynomial equation solver using the GNU Scientific Library (GSL).
 * GSL is available from: "http://www.gnu.org/software/gsl/".
 *
 * Copyright (C) 2008 George Gesslein II
 *
 * To compile:

./c

 * or

cc -O -Wall -o roots roots.c -lgsl -lgslcblas

 * This file is also useful for testing and as an example of using the GSL from C.
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <gsl/gsl_poly.h>
#include <gsl/gsl_errno.h>

#define	true	1
#define	false	0

#define epsilon	0.00000000000005	/* a good value for doubles */

int
main(int argc, char **argv)
{
  int i, highest_power;
  char *cp, *arg;
  double *a, *z;
  gsl_poly_complex_workspace *w;

  if (argc <= 1) {
    fprintf(stderr, "Coefficients must be specified on the command line.\n");

    printf("\nroots version 1.0 - numerical polynomial equation solver\n");
    printf("\nSolves polynomial = 0 when given all real coefficients of the polynomial.\n");
    printf("Double precision floating point arithmetic is used, accurate to 14 digits.\n");
    printf("\nUsage: %s highest-power-coefficient ... constant-term\n", argv[0]);
    printf("\nThe coefficients must be decimal, floating point numbers.\n");
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
      fprintf(stderr, "\"%s\" is not a floating point number.\n", arg);
      exit(2);
    }
    if (errno) {
      fprintf(stderr, "\"%s\" is out of range.\n", arg);
      exit(2);
    }
  }

/* nicely display the actual polynomial equation we are solving */
  printf("The %d approximate floating point solutions of:\n", highest_power);
  for (i = highest_power; i >= 0; i--) {
    if (a[i]) {
      if (i) {
        printf("%+.14g*x", a[i]);
        if (i > 1) {
          printf("^%d", i);
        }
      } else {
        printf("%+.14g", a[i]);
      }
      printf(" ");
    }
  }
  printf("= 0\nare:\n");

/* solve the polynomial equation */
  w = gsl_poly_complex_workspace_alloc(highest_power + 1);
  if (gsl_poly_complex_solve(a, highest_power + 1, w, z) != GSL_SUCCESS) {
    fprintf(stderr, "Approximation failed.\n");
    exit(1);
  }
  gsl_poly_complex_workspace_free(w);

/* display all solutions */
  for (i = 0; i < highest_power; i++) {
#if true /* zero out relatively very small values (which are floating point error) */
    if (fabs(z[2*i] * epsilon) > fabs(z[2*i+1]))
      z[2*i+1] = 0.0;
    else if (fabs(z[2*i+1] * epsilon) > fabs(z[2*i]))
      z[2*i] = 0.0;
#endif
    printf("x = %+.14g", z[2*i]);	/* output real part */
    if (z[2*i+1])
      printf(" %+.14g*i", z[2*i+1]);	/* output imaginary part */
    printf("\n");
  }

  exit(0);
}
