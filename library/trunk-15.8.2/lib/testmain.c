/*
 * This file contains the test/example program
 * for the Mathomatic symbolic math library and API.
 * Copy or refer to this, if you are going to use the Mathomatic code in other projects.
 */

#include <stdio.h>
#include <stdlib.h>
#include "mathomatic.h"

int
main(int argc, char **argv)
{
	char		*cp;		/* character pointer */
	char		*ocp;		/* output character pointer */
	int		rv;		/* return value */
	char		buf[1000];	/* buffer */
	char		*version;	/* version number of the library */

	printf("Mathomatic library test/example program.\n");
	/* Initialize all global variables and arrays so that Mathomatic will work properly. */
	if (!matho_init()) {	/* call this library function exactly once in your program */
		fprintf(stderr, "Not enough memory.\n");
		exit(1);
	}
	/* Mathomatic is ready for use. */
	if (matho_process("version", &version)) {
		printf("Mathomatic library version %s\n", version);
	} else {
		printf("Error getting library version number.\n");
	}

	/* This is a standard input/output loop for testing. */
	printf("Press the EOF character (Control-D) to exit.\n");
	for (;;) {
		printf("%d-> ", cur_equation + 1);
		fflush(stdout);
		if ((cp = fgets(buf, sizeof(buf), stdin)) == NULL)
			break;
		/* Run the Mathomatic symbolic math engine. */
		rv = matho_process(cp, &ocp);
		if (warning_str) {
			/* Optionally display any warnings (not required, but helpful). */
			printf("Warning: %s\n", warning_str);
		}
		if (ocp) {
			if (rv && result_en >= 0) {
				/* Display the result equation number. */
				printf("%d: ", result_en + 1);
			}
			/* Display the result. */
			printf("Library result string:\n%s\n", ocp);
			if (rv) {
				free(ocp);
			} else {
				printf("Error return.\n");
			}
		}
	}
	printf("\n");
	exit(0);
}
