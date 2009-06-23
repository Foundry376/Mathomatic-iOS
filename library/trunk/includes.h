/*
 * Standard C include files for Mathomatic.
 * Automatically includes all necessary C include files for
 * any Mathomatic C source code.
 *
 * Copyright (C) 1987-2009 George Gesslein II.
 */

#define	true	1
#define	false	0

#if	HANDHELD
#undef	UNIX		/* Don't include the extra Unix desktop features. */
#endif

#if	LIBRARY
#define	SILENT	true	/* Stop messages going to stdout. */
#undef	READLINE	/* Readline shouldn't be included in the library code. */
#endif

#if	__CYGWIN__ && !CYGWIN
#warning "Compiling under Cygwin without proper defines.  Please define CYGWIN"
#endif

/* Include files from /usr/include: */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#if	UNIX
#include <libgen.h>
#endif
#include <limits.h>
#include <float.h>
#include <math.h>
#include <setjmp.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#if	I18N
#include <libintl.h>
#endif
#if	READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

/* Include files from the current directory: */
#include "am.h"		/* the main include file for Mathomatic, contains tunable parameters */
#include "complex.h"	/* floating point complex number arithmetic function prototypes */
#include "proto.h"	/* global function prototypes made with cproto utility */
#include "altproto.h"	/* backup global function prototypes */
#include "externs.h"	/* global variable extern definitions */
