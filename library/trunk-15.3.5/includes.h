/*
 * Standard C include files for Mathomatic.
 * Automatically includes all necessary C include files for
 * any Mathomatic C source code.
 *
 * Copyright (C) 1987-2010 George Gesslein II.
 
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

#define	true	1
#define	false	0

#if	HANDHELD
#undef	UNIX		/* Don't include the extra Unix desktop features. */
#endif

#if	LIBRARY
#define	SILENT	true	/* Stop messages going to stdout. */
#undef	READLINE	/* Readline shouldn't be included in the library code. */
#endif

#if 	CYGWIN
#undef	UNIX		/* Unix desktop functionality is covered by defining CYGWIN */
#elif	__CYGWIN__
#warning Compiling under Cygwin without proper defines.
#warning Please define CYGWIN on the compiler command line with -DCYGWIN
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
#include <libintl.h>	/* Mac OS X doesn't have libintl.h, so define "char *gettext();" then. */
#include <locale.h>
#endif

#if	READLINE
#if	!NO_COLOR && !LIBRARY && !CYGWIN	/* The following two includes only needed if explicitly calling ncurses functions. */
#include <curses.h>
#include <term.h>
#endif
#include <readline/readline.h>
#include <readline/history.h>
#endif

/* Include files from the current directory: */
#include "am.h"		/* the main include file for Mathomatic, contains tunable parameters */
#include "complex.h"	/* floating point complex number arithmetic function prototypes */
#include "proto.h"	/* global function prototypes, made with cproto utility */
#include "altproto.h"	/* backup global function prototypes, in case of no proto.h */
#include "externs.h"	/* global variable extern definitions */
