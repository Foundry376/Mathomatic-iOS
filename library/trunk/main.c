/*
 * This file contains main() and the startup code for Mathomatic,
 * which is an algebraic manipulator written entirely in C.
 *
 * Copyright (C) 1987-2008 George Gesslein II.
 *
 * Output to stderr is only done in this file.  The rest of the code
 * should not output to stderr.
 *
 * This program only supports binary and unary operators.
 * Unary operators are implemented as a binary operation with a dummy operand.
 *
 * In the storage format, each level of parentheses is indicated
 * by a level number (origin 1).  The deeper the level, the
 * higher the level number.
 *
 * The storage format for expressions is a fixed size array of elements
 * "token_type", which may be a CONSTANT, VARIABLE, or OPERATOR.
 * The array always alternates between operand (CONSTANT or VARIABLE)
 * and OPERATOR.  There is a separate integer for each array which
 * contains the current length of the expression stored in the array.
 * This length is always odd and must never exceed "n_tokens".
 *
 * Only one POWER operator is allowed per level in the storage format,
 * and no other operators may be on that level.  Same with the FACTORIAL
 * and MODULUS operators.
 *
 * Any number of TIMES and DIVIDE operators may be on the same
 * level, because they are simple multiplicative class operators.
 * The same for PLUS and MINUS, because they are additive class operators.
 *
 * See the file "am.h" to start understanding the Mathomatic code and
 * to adjust memory usage.
 */

#if	!LIBRARY
#include "includes.h"
#if	UNIX || CYGWIN
#include <sys/ioctl.h>
#include <termios.h>
#endif
#if	TRAP_FPERRORS	/* Do not use this define!  Currently does not work. */
#include <fenv.h>
#endif

static void usage();
static int set_signals();

static char	rc_file[MAX_CMD_LEN];

int
main(argc, argv)
int	argc;
char	**argv;
{
	extern char	*optarg;	/* set by getopt(3) */
	extern int	optind;

	int		i;
	char		*cp = NULL;
	double		numerator, denominator;
	double		multiplier;
	int		coption = false;

#if	UNIX
	prog_name = strdup(basename(argv[0]));		/* set prog_name to this executable's filename */
#endif
#if	CYGWIN
	dir_path = strdup(dirname_win(argv[0]));	/* set dir_path to this executable's directory */
#endif
	/* initialize the global variables */
	init_gvars();
	gfp = stdout;
#if	READLINE
	using_history();		/* initialize readline history */
	stifle_history(500);		/* maximum of 500 entries */
	rl_inhibit_completion = true;	/* turn off readline file name completion */
#endif

	/* process command line options */
	while ((i = getopt(argc, argv, "qrtchuvm:")) >= 0) {
		switch (i) {
		case 'c':
			coption++;
			break;
		case 'h':
			html_flag = true;
			break;
		case 'm':
			multiplier = strtod(optarg, &cp) * DEFAULT_N_TOKENS;
			if (cp == NULL || *cp || multiplier < 100 || multiplier >= (INT_MAX / 3)) {
				fprintf(stderr, _("%s: Invalid memory size multiplier specified.\n"), prog_name);
				exit(2);
			}
			n_tokens = (int) multiplier;
			break;
		case 'q':
			quiet_mode = true;
			break;
		case 'r':
			readline_enabled = false;
			break;
		case 't':
			readline_enabled = false;
			test_mode = true;
			break;
		case 'u':
			setbuf(stdout, NULL);	/* make output unbuffered */
			setbuf(stderr, NULL);
			break;
		case 'v':
			version_report();
			exit(0);
		default:
			usage();
		}
	}
	if (test_mode || html_flag) {
		screen_columns = 0;
		screen_rows = 0;
#if	UNIX || CYGWIN
	} else {
		get_screen_size();
#endif
	}
	if (!init_mem()) {
		fprintf(stderr, _("%s: Not enough memory.\n"), prog_name);
		exit(2);
	}
#if	READLINE && !SECURE
	if (readline_enabled) {
		read_history(NULL);	/* restore readline history of previous session */
	}
#endif
	if (html_flag) {
		printf("<pre>\n");
	}
	if (!test_mode && !quiet_mode) {
#if	SECURE
		printf(_("Secure "));
#endif
		printf(_("This is Mathomatic version %s (www.mathomatic.org).\n"), VERSION);
		printf(_("Copyright (C) 1987-2008 George Gesslein II.\n"));
		printf(_("%d equation spaces available, %ldK per equation space.\n"),
		    N_EQUATIONS, (long) n_tokens * sizeof(token_type) * 2L / 1000L);
	}
#if	!SECURE
	/* read the user options initialization file */
	if (!test_mode && !load_rc()) {
		fprintf(stderr, _("%s: Error loading set options from \"%s\".\n"), prog_name, rc_file);
	}
#endif
	if (test_mode) {
		color_flag = false;
	} else if (coption & 1) {
		color_flag = !color_flag;
	}
	if (!quiet_mode && color_flag) {
		printf(_("%s%s color mode enabled.\n"), html_flag ? "HTML" : "ANSI", bold_colors ? " bold" : "");
	}
	if ((i = setjmp(jmp_save)) != 0) {
		/* for error handling */
		clean_up();
		switch (i) {
		case 14:
			error(_("Expression too large."));
		default:
			printf(_("Operation aborted.\n"));
			break;
		}
	} else {
		set_signals();
		if (!f_to_fraction(0.5, &numerator, &denominator)
		    || numerator != 1.0 || denominator != 2.0
		    || !f_to_fraction(1.0/3.0, &numerator, &denominator)
		    || numerator != 1.0 || denominator != 3.0) {
			fprintf(stderr, _("%s: Cannot convert any floating point values to fractions.\n"), prog_name);
			fprintf(stderr, _("Roots will not work properly.\n"));
		}
#if	!SECURE
		/* read in files on the command line, exit if error */
		for (i = optind; i < argc; i++) {
			if (!read_cmd(argv[i])) {
				exit_program(1);
			}
		}
#endif
	}
	/* main input/output loop */
	for (;;) {
		default_color();
		snprintf(prompt_str, sizeof(prompt_str), "%d%s", cur_equation + 1, html_flag ? HTML_PROMPT : PROMPT);
		if ((cp = get_string((char *) tlhs, n_tokens * sizeof(token_type))) == NULL)
			break;
		process(cp);
	}
	exit_program(0);
	return(0);	/* so the compiler doesn't complain */
}

/*
 * Display invocation usage info and exit with error.
 */
static void
usage()
{
	printf(_("Mathomatic version %s - automatic algebraic manipulator\n\n"), VERSION);
	printf(_("Usage: %s [ options ] [ input_files ]\n\n"), prog_name);
	printf(_("Options:\n"));
	printf(_("  -c            Toggle color mode.\n"));
	printf(_("  -h            Enable HTML output mode.\n"));
	printf(_("  -m number     Specify a memory size multiplier.\n"));
	printf(_("  -q            Set quiet mode (don't display prompts).\n"));
	printf(_("  -r            Disable readline.\n"));
	printf(_("  -t            Set test mode.\n"));
	printf(_("  -u            Set unbuffered output.\n"));
	printf(_("  -v            Display version number, etc., then exit.\n"));
	printf(_("\nPlease refer to the man page for more information.\n"));
	exit(2);
}

/*
 * All signal(2) initialization goes here.
 *
 * Return true on success.
 */
static int
set_signals()
{
	int	rv = true;

	if (signal(SIGFPE, fphandler) == SIG_ERR)
		rv = false;
#if	TRAP_FPERRORS
	if (feenableexcept(FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW) < 0)
		rv = false;
#endif
	if (signal(SIGINT, inthandler) == SIG_ERR)
		rv = false;
#if	UNIX || CYGWIN
	if (signal(SIGWINCH, resizehandler) == SIG_ERR)
		rv = false;
#endif
#if	TIMEOUT_SECONDS
	if (signal(SIGALRM, alarmhandler) == SIG_ERR)
		rv = false;
	alarm(TIMEOUT_SECONDS);
#endif
	if (!rv) {
		perror("signal(2) setting failed");
	}
	return rv;
}

#if	CYGWIN
/*
 * dirname(3) function for Windows.
 */
char	*
dirname_win(cp)
char	*cp;
{
	int	i;

	i = strlen(cp);
	while (i >= 0 && cp[i] != '\\' && cp[i] != '/')
		i--;
	if (i < 0)
		return(".");
	cp[i] = '\0';
	return(cp);
}
#endif

#if	!SECURE
/*
 * Load set options from "~/.mathomaticrc".
 *
 * Return false if error encountered.
 */
int
load_rc()
{
	FILE	*fp = NULL;
	char	buf[MAX_CMD_LEN];
	char	*cp;
	int	rv = true;

	cp = getenv("HOME");
	if (cp) {
		snprintf(rc_file, sizeof(rc_file), "%s/%s", cp, ".mathomaticrc");
		fp = fopen(rc_file, "r");
	}
#if	CYGWIN
	if (fp == NULL) {
		snprintf(rc_file, sizeof(rc_file), "%s/%s", dir_path, "mathomatic.rc");
		fp = fopen(rc_file, "r");
	}
#endif
	if (fp == NULL) {
		rc_file[0] = '\0';
		return true;
	}
	while ((cp = fgets(buf, sizeof(buf), fp)) != NULL) {
		set_error_level(cp);
		if (!set_options(cp))
			rv = false;
	}
	fclose(fp);
	return rv;
}
#endif

/*
 * Floating point exception handler.
 * I do not know the proper function calls that should be here to make the GNU TRAP_FPERRORS define work.
 * Currently it goes in an endless loop when an FP exception occurs and TRAP_FPERRORS is true,
 * because when this function returns, the exception immediately happens again.
 * Therefore, do not define TRAP_FPERRORS!
 */
void
fphandler(sig)
int	sig;
{
#if	TRAP_FPERRORS
	warning(_("Floating point exception."));
/*
	feenableexcept(FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW);
	signal(SIGFPE, fphandler);
*/
#endif
}

/*
 * Control-C (interrupt) signal handler.
 * Interrupts processing and returns to main prompt through a polling mechanism.
 * If it can't, repeated calls terminate this program.
 */
void
inthandler(sig)
int	sig;
{
	abort_flag++;
	switch (abort_flag) {
	case 0:
	case 1:
		/* wait for graceful abort */
		return;
	case 2:
		printf(_("\nPress Control-C once more to quit program.\n"));
		break;
	default:
		/* abruptly quit this program */
		printf(_("\nControl-C repeatedly pressed; Quitting...\n"));
		exit_program(1);
	}
}

/*
 * Alarm signal handler.
 */
void
alarmhandler(sig)
int	sig;
{
	printf(_("\nTimeout, quitting...\n"));
	exit_program(1);
}

#if	UNIX || CYGWIN
/*
 * Get the screen (window) width and height from the operating system.
 */
void
get_screen_size()
{
	struct winsize	ws;

	ws.ws_col = 0;
	ws.ws_row = 0;
	if (ioctl(1, TIOCGWINSZ, &ws) >= 0) {
		if (ws.ws_col > 0 && screen_columns) {
			screen_columns = ws.ws_col;
		}
		if (ws.ws_row > 0 && screen_rows) {
			screen_rows = ws.ws_row;
		}
	}
}

/*
 * Window resize signal handler.
 */
void
resizehandler(sig)
int	sig;
{
	get_screen_size();
}
#endif

/*
 * Exit this program and return to the Operating System.
 */
void
exit_program(exit_value)
int	exit_value;	/* zero if OK, non-zero indicates error return */
{
	reset_attr();
	if (html_flag) {
		printf("</pre>\n");
	}
#if	READLINE && !SECURE
	if (readline_enabled) {
		write_history(NULL);	/* save readline history in ~/.history */
	}
#endif
	exit(exit_value);
}
#endif
