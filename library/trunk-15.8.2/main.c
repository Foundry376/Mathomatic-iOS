/*
 * This file contains main() and the startup code for Mathomatic,
 * which is a computer algebra system written in the C programming language.
 *
 * Copyright (C) 1987-2012 George Gesslein II.
 
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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

The chief copyright holder can be contacted at gesslein@mathomatic.org, or
George Gesslein II, P.O. Box 224, Lansing, NY  14882-0224  USA.
 
 */

/*
 * Output to stderr is only done in this file.  The rest of the Mathomatic code
 * should not output to stderr; error messages should use error() or go to stdout.
 * One reason for this is so that Mathomatic stdout can be redirected or piped,
 * catching all output.
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
 * In the storage format,
 * any number of TIMES and DIVIDE operators may be on the same level of parentheses,
 * because they are similar and the most basic multiplicative class operators.
 * The same for the PLUS and MINUS operators, because they are similar (additive class).
 * All other operators are only allowed one single operator per level of parentheses,
 * and no same nor different operators may be with it on that level within the current grouping.
 *
 * Most of the expression manipulation and comparison routines are recursive,
 * calling themselves for each level of parentheses.
 *
 * Note that equation space numbers internally are 1 less than the equation
 * space numbers displayed.  That is because internal equation space numbers
 * are origin 0 array indexes, while displayed equation numbers are origin 1.
 *
 * See the file "am.h" to start understanding the Mathomatic code and
 * to adjust memory usage.
 */

#if	!LIBRARY	/* This comments out this whole file if compiling as the symbolic math library. */

#include "includes.h"
#if	!NO_GETOPT_H
#include <getopt.h>
#endif

#if	WIN32_CONSOLE_COLORS
#include <windows.h>
#include <wincon.h>

HANDLE	hOut;
#endif

void
display_startup_message(fp)
FILE	*fp;
{
#if	SECURE
	fprintf(fp, _("Secure "));
#else
	if (security_level >= 2)
		fprintf(fp, _("Secure "));
	else if (security_level == -1)
		fprintf(fp, "m4 ");
#endif
	fprintf(fp, "Mathomatic version %s\n", VERSION);
	if (html_flag)
		fprintf(fp, "Copyright &copy; 1987-2012 George Gesslein II.\n");
	else
		fprintf(fp, "Copyright (C) 1987-2012 George Gesslein II.\n");
	fprintf(fp, _("%d equation spaces available in memory,\n%ld kilobytes per equation space.\n"),
	    N_EQUATIONS, (long) n_tokens * sizeof(token_type) * 2L / 1000L);
}

/*
 * Display invocation usage info.
 */
void
usage(fp)
FILE	*fp;
{
	fprintf(fp, _("Mathomatic computer algebra system, version %s\n"), VERSION);
	fprintf(fp, _("Usage: %s [ options ] [ input_files or input ]\n\n"), prog_name);
	fprintf(fp, _("Options:\n"));
	fprintf(fp, _("  -b            Enable bold color mode.\n"));
	fprintf(fp, _("  -c            Toggle color mode.\n"));
	fprintf(fp, _("  -e            Process expressions and commands on the command line.\n"));
	fprintf(fp, _("  -h            Display this help and exit.\n"));
	fprintf(fp, _("  -m number     Specify a memory size multiplier.\n"));
	fprintf(fp, _("  -q            Set quiet mode (don't display prompts).\n"));
	fprintf(fp, _("  -r            Disable readline.\n"));
	fprintf(fp, _("  -s level      Set enforced security level for session.\n"));
	fprintf(fp, _("  -t            Set test mode.\n"));
	fprintf(fp, _("  -u            Set unbuffered output with input echo.\n"));
	fprintf(fp, _("  -v            Display version information, then exit.\n"));
	fprintf(fp, _("  -w            Wide output mode, sets unlimited width.\n"));
	fprintf(fp, _("  -x            Enable HTML/XHTML output mode.\n"));
	fprintf(fp, _("\nPlease refer to the Mathomatic man page for details.\n"));
	fprintf(fp, _("The man page is viewed by typing \"man mathomatic\" in shell.\n"));
}

int
main(argc, argv)
int	argc;
char	**argv;
{
#if	NO_GETOPT_H	/* if no getopt.h is available */
	extern char	*optarg;	/* set by getopt(3) */
	extern int	optind;
#endif

	int		i;
	char		*cp = NULL;
	double		numerator, denominator;
	double		new_size = 0;
	int		coption = false, boption = false, wide_flag = false;
	int		exit_value = 0;

#if	WIN32_CONSOLE_COLORS
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
#endif

#if	I18N
	/* Initialize internationalization so that messages are in the right language. */
	setlocale(LC_ALL, "");
	bindtextdomain(prog_name, LOCALEDIR);
	textdomain(prog_name);
#endif

#if	CYGWIN || MINGW
	dir_path = strdup(dirname_win(argv[0]));	/* set dir_path to this executable's directory */
#endif
	/* initialize the global variables */
	init_gvars();
	default_out = stdout;	/* set default_out to any file you want to redirect output to */
	gfp = default_out;
	get_screen_size();

	/* process command line options */
	while ((i = getopt(argc, argv, "s:bqrtdchuvwxm:e")) >= 0) {
		switch (i) {
		case 's':
#if	SECURE
			fprintf(stderr, _("%s: Compiled for maximum security, therefore no need for setting security level.\n"), prog_name);
#else
			if (optarg)
				security_level = strtod(optarg, &cp);
			if (optarg == NULL || cp == NULL || cp == optarg || *cp) {
				fprintf(stderr, _("%s: Error in setting security level.\n"), prog_name);
				exit(2);
			}
#endif
			break;
		case 'w':
			wide_flag = true;
			break;
		case 'b':
			boption = true;
			break;
		case 'c':
			coption++;
			break;
		case 'x':
			html_flag = true;
			wide_flag = true;
			break;
		case 'm':
			if (optarg)
				new_size = strtod(optarg, &cp) * DEFAULT_N_TOKENS;
			if (optarg == NULL || cp == NULL || *cp || new_size <= 0 || new_size >= (INT_MAX / sizeof(token_type))) {
				fprintf(stderr, _("%s: Invalid memory size multiplier specified.\n"), prog_name);
				exit(2);
			}
			n_tokens = (int) new_size;
			break;
		case 'q':
			quiet_mode = true;
			break;
		case 'r':
			readline_enabled = false;
			break;
		case 't':
			readline_enabled = false;
			wide_flag = true;
			test_mode = true;
			break;
		case 'd':
			demo_mode = true;
			break;
		case 'u':
			echo_input = true;
			setbuf(stdout, NULL);	/* make output unbuffered */
			setbuf(stderr, NULL);
			break;
		case 'h':
			usage(stdout);
			exit(0);
		case 'v':
			version_report();
			exit(0);
		case 'e':
			eoption = true;
			autoselect = false;
			break;
		default:
			usage(stdout);
			exit(2);
		}
	}
	if (n_tokens < 100 || n_tokens >= (INT_MAX / sizeof(token_type))) {
		fprintf(stderr, _("%s: Standard expression array size %d out of range!\n"), prog_name, n_tokens);
	}
	if (!init_mem()) {
		fprintf(stderr, _("%s: Not enough memory.\n"), prog_name);
		exit(2);
	}
#if	READLINE
	if (readline_enabled) {	/* readline_enabled flag must not change after this */
		if ((cp = getenv("HOME")) && prog_name) {
#if	CYGWIN || MINGW
			snprintf(history_filename_storage, sizeof(history_filename_storage), "%s/%s_history", cp, prog_name);
#else
			snprintf(history_filename_storage, sizeof(history_filename_storage), "%s/.%s_history", cp, prog_name);
#endif
			history_filename = history_filename_storage;
		}
		using_history();		/* initialize readline history */
		rl_initialize();		/* initialize readline */
		stifle_history(500);		/* maximum of 500 entries */
		rl_inhibit_completion = true;	/* turn off readline file name completion */
#if	0	/* not 100% tested and this might confuse the user with the -c toggle */
#if	!WIN32_CONSOLE_COLORS
		if (!html_flag) {		/* If doing ANSI color: */
			color_flag = (tigetnum("colors") >= 8);	/* autoset color output mode.  Requires ncurses. */
		}
#endif
#endif
#if	!SECURE
		if (security_level <= 3) {
			read_history(history_filename);	/* restore readline history of previous session */
		}
#endif
	}
#endif
	if (html_flag) {
		printf("<pre>\n");
	}
	if (!test_mode && !quiet_mode && !eoption) {
		display_startup_message(stdout);
	}
	fflush(stdout);
#if	!SECURE
	/* read the user options initialization file */
	if (security_level <= 3 && !test_mode && !demo_mode && !load_rc()) {
		fprintf(stderr, _("%s: Error loading set options from \"%s\".\n\n"), prog_name, rc_file);
		snprintf(prompt_str, sizeof(prompt_str), _("Remove faulty set options startup file (y/n)? "));
		if (get_yes_no()) {
			if (unlink(rc_file) == 0) {
				printf(_("File \"%s\" removed.\n"), rc_file);
				printf(_("Next startup of Mathomatic should be clean with the defaults set.\n"));
			} else {
				perror(rc_file);
			}
		}
		printf("\n");
	}
#endif
	if (wide_flag) {
		screen_columns = 0;
		screen_rows = 0;
	}
	if (coption & 1) {
		color_flag = !color_flag;
	}
	if (boption) {
		color_flag = true;
		bold_colors = true;
	}
	if (test_mode) {
		color_flag = false;
	} else if (!quiet_mode && !eoption) {
		if (color_flag) {
#if	WIN32_CONSOLE_COLORS
			printf(_("%s%s color mode enabled"), html_flag ? "HTML" : "WIN32 CONSOLE", bold_colors ? " bold" : "");
#else
			printf(_("%s%s color mode enabled"), html_flag ? "HTML" : "ANSI", bold_colors ? " bold" : "");
#endif
			printf(_("; manage by typing \"help color\".\n"));
		} else {
			printf(_("Color mode turned off; manage it by typing \"help color\".\n"));
		}
	}
	fflush(stdout);
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
		if (eoption)
			exit_value = 1;
	} else {
		if (!set_signals()) {
			fprintf(stderr, _("signal(2) setting failed.\n"));
		}
		if (!f_to_fraction(0.5, &numerator, &denominator)
		    || numerator != 1.0 || denominator != 2.0
		    || !f_to_fraction(1.0/3.0, &numerator, &denominator)
		    || numerator != 1.0 || denominator != 3.0) {
			fprintf(stderr, _("%s: Cannot convert any floating point values to fractions.\n"), prog_name);
			fprintf(stderr, _("Roots will not work properly.\n"));
		}
		if (max_memory_usage() <= 0) {
			fprintf(stderr, _("%s: Calculated maximum memory usage overflows a long integer!\n"), prog_name);
		}
		if (eoption) {
			/* process expressions and commands from the command line */
			for (i = optind; i < argc && argv[i]; i++) {
				if (!display_process(argv[i])) {
					exit_value = 1;
				}
			}
		} else {
#if	SECURE
			if (!html_flag && !quiet_mode && !test_mode)
				printf(_("Anything done here is temporary.\n"));
			if (optind < argc) {
				warning(_("Command-line argument ignored."));
			}
#else
			if (!html_flag && !quiet_mode && !test_mode)
				printf(_("Anything done here is temporary, unless it is saved or redirected.\n"));
			/* read in files specified on the command line, exit if error */
			for (i = optind; i < argc && argv[i]; i++) {
				if (strcmp(argv[i], "-") == 0) {
					main_io_loop();
				} else if (!read_file(argv[i])) {
					fflush(NULL);	/* flush all output */
					fprintf(stderr, _("Read of file \"%s\" failed.\n"), argv[i]);
					exit_program(1);
				}
			}
#endif
		}
	}
	if (!eoption)
		main_io_loop();		/* main input/output loop */
	exit_program(exit_value);	/* exit Mathomatic, doesn't return */
	return(exit_value);		/* so the compiler doesn't complain */
}

/*
 * Repeatedly read a line of text from standard input and process the expression or command.
 */
void
main_io_loop(void)
{
	char	*cp = NULL;

	for (;;) {
		default_color();
		snprintf(prompt_str, sizeof(prompt_str), "%d%s", cur_equation + 1, html_flag ? HTML_PROMPT_STR : PROMPT_STR);
		if ((cp = get_string((char *) tlhs, n_tokens * sizeof(token_type))) == NULL)
			break;
		process(cp);
	}
}

/*
 * All signal(2) initialization goes here.
 *
 * Return true on success.
 */
int
set_signals(void)
{
	int	rv = true;

	if (signal(SIGFPE, fphandler) == SIG_ERR)
		rv = false;
	if (signal(SIGINT, inthandler) == SIG_ERR)
		rv = false;
	if (signal(SIGTERM, exithandler) == SIG_ERR)
		rv = false;
#if	!MINGW
	if (signal(SIGHUP, exithandler) == SIG_ERR)
		rv = false;
#endif
#if	UNIX || CYGWIN
	if (signal(SIGWINCH, resizehandler) == SIG_ERR)
		rv = false;
#endif
#if	TIMEOUT_SECONDS
	if (signal(SIGALRM, alarmhandler) == SIG_ERR)
		rv = false;
	alarm(TIMEOUT_SECONDS);
#endif
	return rv;
}

#if	CYGWIN || MINGW
/*
 * dirname(3) function for Microsoft Windows.
 * dirname(3) strips the non-directory suffix from a filename.
 */
char *
dirname_win(cp)
char	*cp;	/* string containing filename to modify */
{
	int	i;

	if (cp == NULL)
		return(".");
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
 * Load set options from startup file "~/.mathomaticrc".
 *
 * Return false if there was an error reading the startup file,
 * otherwise return true.
 */
int
load_rc(void)
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
#if	CYGWIN || MINGW
	if (fp == NULL && cp) {
		snprintf(rc_file, sizeof(rc_file), "%s/%s", cp, "mathomatic.rc");
		fp = fopen(rc_file, "r");
	}
	if (fp == NULL && dir_path) {
		snprintf(rc_file, sizeof(rc_file), "%s/%s", dir_path, "mathomatic.rc");
		fp = fopen(rc_file, "r");
	}
#endif
	if (fp == NULL) {
		return true;
	}
	if (!quiet_mode && !eoption) {
		printf(_("Loading startup set options from \"%s\".\n"), rc_file);
	}
	while ((cp = fgets(buf, sizeof(buf), fp)) != NULL) {
		set_error_level(cp);
		if (!set_options(cp))
			rv = false;
	}
	if (fclose(fp)) {
		rv = false;
		perror(rc_file);
	}
	return rv;
}
#endif

/*
 * Floating point exception handler.
 * Floating point exceptions are currently ignored.
 */
void
fphandler(sig)
int	sig;
{
#if	DEBUG
	warning("Floating point exception.");
#endif
}

/*
 * Fancy Control-C (interrupt) signal handler.
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
		printf(_("\nUser interrupt signal received; three times quits Mathomatic.\n"));
		return;
	case 2:
		printf(_("\nPress Control-C once more to quit program.\n"));
		return;
	default:
		/* abruptly quit this program */
		printf(_("\nRepeatedly interrupted; returning to operating system...\n"));
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

/*
 * Signal handler for proper exiting to the operating system.
 */
void
exithandler(sig)
int	sig;
{
	exit_program(1);
}

#if	UNIX || CYGWIN
/*
 * Window resize signal handler.
 */
void
resizehandler(sig)
int	sig;
{
	if (screen_columns)
		get_screen_size();
}
#endif

/*
 * Properly exit this program and return to the operating system.
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
	if (readline_enabled && security_level <= 3) {
		write_history(history_filename);	/* save readline history */
	}
#endif
	if (exit_value == 0 && !quiet_mode && !eoption && !html_flag) {
		printf(_("ByeBye!! from Mathomatic.\n"));
	}
	exit(exit_value);
}
#endif
