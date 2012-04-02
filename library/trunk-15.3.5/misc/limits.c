/*
 * Standalone program to display current C data type limits and sizes.
 *
 * Compile with:
 *
 * cc limits.c -o limits
 *
 * then type "./limits".
 */

#include <stdio.h>
#include <limits.h>
#include <float.h>

int main(void)
{
  printf("Integer limits:\n\n");
  printf("Bits of type char: %d\n", CHAR_BIT);
  printf("sizeof(char) = %u bytes.\n", (unsigned) sizeof(char));
  printf("sizeof(char *) = %u bytes.\n", (unsigned) sizeof(char *));

  if (CHAR_MIN < 0) {
    printf("Current type char is signed.\n");
  } else {
    printf("Current type char is unsigned.\n");
  }
  printf("Maximum numeric value of type char: %d\n", CHAR_MAX);
  printf("Minimum numeric value of type char: %d\n\n", CHAR_MIN);

  printf("Maximum value of type signed char: %d\n", SCHAR_MAX);
  printf("Minimum value of type signed char: %d\n\n", SCHAR_MIN);

  printf("Maximum value of type unsigned char: %u\n\n", (unsigned) UCHAR_MAX);

  printf("sizeof(short) = %u bytes.\n", (unsigned) sizeof(short));
  printf("Maximum value of type short: %d\n", SHRT_MAX);
  printf("Minimum value of type short: %d\n\n", SHRT_MIN);

  printf("Maximum value of type unsigned short: %u\n\n", (unsigned) USHRT_MAX);

  printf("sizeof(int) = %u bytes.\n", (unsigned) sizeof(int));
  printf("Maximum value of type int: %d\n", INT_MAX);
  printf("Minimum value of type int: %d\n\n", INT_MIN);

  printf("Maximum value of type unsigned int: %u\n\n", UINT_MAX);

  printf("sizeof(long) = %u bytes.\n", (unsigned) sizeof(long));
  printf("Maximum value of type long: %ld\n", LONG_MAX);
  printf("Minimum value of type long: %ld\n\n", LONG_MIN);

  printf("Maximum value of type unsigned long: %lu\n\n", ULONG_MAX);

#ifdef	ULLONG_MAX
  printf("sizeof(long long) = %u bytes.\n", (unsigned) sizeof(long long));
  printf("Maximum value of type long long: %lld\n", LLONG_MAX);
  printf("Minimum value of type long long: %lld\n\n", LLONG_MIN);

  printf("Maximum value of type unsigned long long: %llu\n\n", ULLONG_MAX);
#endif

  printf("Floating point limits:\n\n");

  printf("sizeof(float) = %u bytes.\n", (unsigned) sizeof(float));
  printf("Decimal digits of precision for float: %d\n", FLT_DIG);
  printf("sizeof(double) = %u bytes.\n", (unsigned) sizeof(double));
  printf("Decimal digits of precision for double: %d\n", DBL_DIG);
  printf("sizeof(long double) = %u bytes.\n", (unsigned) sizeof(long double));
  printf("Decimal digits of precision for long double: %d\n\n", LDBL_DIG);

  printf("Maximum value of float: %g\n", FLT_MAX);
  printf("Minimum value of float: %g\n\n", FLT_MIN);

  printf("Maximum value of double: %g\n", DBL_MAX);
  printf("Minimum value of double: %g\n\n", DBL_MIN);

  printf("Maximum value of long double: %Lg\n", LDBL_MAX);
  printf("Minimum value of long double: %Lg\n\n", LDBL_MIN);

  printf("Epsilon is the smallest x such that 1.0 + x != 1.0\n");
  printf("Float epsilon: %g\n", FLT_EPSILON);
  printf("Double epsilon: %g\n", DBL_EPSILON);
  printf("Long double epsilon: %Lg\n", LDBL_EPSILON);

  printf("\nEnd of program output.\n");

  return 0;
}
