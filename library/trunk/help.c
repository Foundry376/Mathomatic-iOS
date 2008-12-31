/*
 * Mathomatic help command and parsing routines.
 *
 * Everything that depends on the command table goes here.
 *
 * Copyright (C) 1987-2008 George Gesslein II.
 */

#include "includes.h"

/*
 * The following structure is used for each Mathomatic command.
 */
typedef	struct {
	char	*name;			/* command name to be typed by user (must not contain any spaces) */
	char	*secondary_name;	/* another name for this command */
	int	(*func)();		/* function that handles this command */
					/* function is passed a char pointer and returns true if successful */
	char	*usage;			/* command syntax text */
	char	*info;			/* one line description of command */
	char	*extra;			/* one line extra info on command */
} com_type;

/*
 * The Mathomatic command table follows.  It should be in alphabetical order.
 */
static com_type com_list[] = {
/*	command name,	alternate name,		function,		usage,							information */
{	"approximate",	NULL,			approximate_cmd,	"[equation-number-ranges]",				"Approximate all numerical values in equation spaces." },
#if	!LIBRARY
{	"calculate",	NULL,			calculate_cmd,		"[\"repeat\"] [\"factor\"] [variable iterations]",	"Temporarily plug in values for variables and approximate." },
#endif
{	"clear",	NULL,			clear_cmd,		"[equation-number-ranges]",				"Delete expressions stored in memory so equation spaces can be reused.", "Tip: Use \"clear all\" to restart Mathomatic." },
#if	!LIBRARY
{	"code",		NULL,			code_cmd,		"[\"c\" or \"java\" or \"python\" or \"integer\"] [equation-number-ranges]",	"Output C, Java, or Python code for the specified equations.", "Related commands: simplify, optimize, and variables." },
#endif
{	"compare",	NULL,			compare_cmd,		"[\"symbolic\"] equation-number [\"with\" equation-number]",	"Compare two equation spaces to see if they are the same." },
{	"copy",		NULL,			copy_cmd,		"[equation-number-range]",				"Duplicate the specified equation spaces." },
{	"derivative",	"differentiate",	derivative_cmd,		"[\"nosimplify\"] [variable or \"all\"] [order]",	"Symbolically differentiate and simplify, order times." },
{	"display",	NULL,			display_cmd,		"[\"factor\"] [equation-number-ranges]",			"Display equation spaces in pretty multi-line (2D) fraction format." },
#if	!LIBRARY
{	"divide",	NULL,			divide_cmd,		"[variable]",						"Prompt for 2 numbers or polynomials and divide. Give result and GCD." },
#endif
{	"echo",		NULL,			echo_cmd,		"[text]",						"Output a line of text, followed by a newline." },
#if	!LIBRARY && (UNIX || CYGWIN) && !SECURE
{	"edit",		NULL,			edit_cmd,		"[file-name]",						"Edit all equation spaces or an input file." },
#endif
{	"eliminate",	NULL,			eliminate_cmd,		"[\"repeat\"] variables or \"all\" [\"using\" equation-number]",	"Substitute the specified variables with solved equations." },
{	"extrema",	NULL,			extrema_cmd,		"[variable] [order]",					"Find possible local minimums and maximums of the current expression." },
{	"factor",	NULL,			factor_cmd,		"[\"numbers\" [integers]] or [equation-number-range] [variables]",	"Factor given integers or factor variables in equation spaces." },
{	"fraction",	NULL,			fraction_cmd,		"[equation-number-range]",				"Convert expressions with algebraic fractions into a single simple fraction." },
{	"help",		NULL,			help_cmd,		"[topic or command-names]",				"Short, built-in help and reference." },
{	"imaginary",	NULL,			imaginary_cmd,		"[variable]",						"Copy the imaginary part of the current expression.", "Related command: real." },
{	"integrate",	"integral",		integrate_cmd,		"[\"constant\" or \"definite\"] variable [order]",	"Symbolically integrate polynomials order times, then simplify." },
{	"laplace",	NULL,			laplace_cmd,		"[\"inverse\"] variable",				"Compute the Laplace or inverse Laplace transform of polynomials." },
{	"limit",	NULL,			limit_cmd,		"variable expression",					"Take the limit as variable goes to expression (experimental)." },
{	"list",		NULL,			list_cmd,		"[\"export\" or \"maxima\"] [equation-number-ranges]",	"Display equation spaces in single-line format." },
#if	!LIBRARY
{	"nintegrate",	NULL,			nintegrate_cmd,		"[\"trapezoid\"] variable [partitions]",		"Do numerical definite integration using Simpson's rule."},
{	"optimize",	NULL,			optimize_cmd,		"[equation-number-range]",				"Split up equations into smaller, more efficient equations.", "Related command: code." },
#endif
#if	!LIBRARY
{	"pause",	NULL,			pause_cmd,		"[text]",						"Wait for user to press the Enter key. Optionally display a message." },
#endif
{	"product",	NULL,			product_cmd,		"variable start end [step-size]",			"Compute the product as variable goes from start to end.", "Related command: sum." },
#if	READLINE
{	"push",		NULL,			push_cmd,		"[equation-number-range]",				"Push the specified expressions into the readline history." },
#endif
#if	!LIBRARY
{	"quit",		"exit",			quit_cmd,		"[exit-value]",						"Terminate this program without saving." },
#endif
#if	!SECURE
{	"read",		NULL,			read_cmd,		"file-name",						"Read in a text file as if it was typed in." },
#endif
{	"real",		NULL,			real_cmd,		"[variable]",						"Copy the real part of the current expression.", "Related command: imaginary." },
{	"replace",	NULL,			replace_cmd,		"[variables [\"with\" expression]]",			"Substitute variables in the current equation with expressions." },
#if	!LIBRARY
{	"roots",	NULL,			roots_cmd,		"root real-part imaginary-part",			"Display all the roots of a complex number." },
#endif
#if	!SECURE
{	"save",		NULL,			save_cmd,		"file-name",						"Save all equation spaces in a text file.", "Related command: read." },
#endif
{	"set",		NULL,			set_cmd,		"[[\"no\"] option]",					"Set or display various session options." },
{	"simplify",	NULL,			simplify_cmd,		"[\"sign\"] [\"symbolic\"] [\"quick\"] [\"fraction\"] [equation-number-range]",	"Completely simplify expressions." },
{	"solve",	NULL,			solve_cmd,		"[\"verify\"] variable or \"0\"",			"Solve the current equation for a variable or for zero." },
{	"sum",		NULL,			sum_cmd,		"variable start end [step-size]",			"Compute the summation as variable goes from start to end.", "Related command: product." },
#if	!LIBRARY
{	"tally",	NULL,			tally_cmd,		"[\"average\"]",					"Prompt for and add entries, show total and optionally the average." },
#endif
{	"taylor",	NULL,			taylor_cmd,		"variable order point",					"Compute the Taylor series expansion of the current expression." },
{	"unfactor",	"expand",		unfactor_cmd,		"[\"fully\"] [\"quick\"] [\"power\"] [equation-number-range]",	"Algebraically expand (multiply out) expressions." },
#if	!LIBRARY
{	"variables",	NULL,			variables_cmd,		"[\"c\" or \"java\" or \"integer\"] [equation-number-range]",	"Output all variable names within the specified equations.", "Related command: code." },
#endif
{	"version",	NULL,			version_cmd,		"",							"Display version number, compile flags, and memory usage." }
};

/*
 * Process mathematical expression input in Mathomatic, with no solving and no automatic calculation.
 *
 * Simply parse the equation or expression text in "cp" and place the result in equation space "n".
 *
 * Return true if successful.
 */
int
parse(n, cp)
int	n;
char	*cp;
{
	if (parse_equation(n, cp)) {
		if (n_lhs[n] == 0 && n_rhs[n] == 0)
			return true;
		if (n_lhs[n] == 0) {
			/* RHS expression only, set equal to zero */
			n_lhs[n] = 1;
			lhs[n][0] = zero_token;
		}
		cur_equation = n;
		return return_result(cur_equation);
	}
	n_lhs[n] = 0;
	n_rhs[n] = 0;
	return false;
}

/*
 * Process equation and expression input in Mathomatic,
 * either swapping, selecting, storing, solving (autosolve), or calculating (autocalc).
 *
 * Parse the mathematical expression text in "cp" and solve the current equation for it
 * or place it in equation space "n" if it is not a solve variable.
 *
 * Return true if successful.
 */
int
process_parse(n, cp)
int	n;
char	*cp;
{
	int	i;

	if (parse_equation(n, cp)) {
		if (n_lhs[n] == 0 && n_rhs[n] == 0) {
			if (strcmp(cp, "=") == 0 && n_lhs[cur_equation] > 0 && n_rhs[cur_equation] > 0) {
				debug_string(0, _("Swapping sides of the current equation..."));
				n = cur_equation;
				i = n_lhs[n];
				blt(tes, lhs[n], n_lhs[n] * sizeof(token_type));
				n_lhs[n] = n_rhs[n];
				blt(lhs[n], rhs[n], n_rhs[n] * sizeof(token_type));
				n_rhs[n] = i;
				blt(rhs[n], tes, i * sizeof(token_type));
				return return_result(cur_equation);
			}
			return true;
		}
		if (n_lhs[n] == 0 || n_rhs[n] == 0) {
			if (autosolve) {
				if ((n_lhs[n] == 1 && ((lhs[n][0].kind == CONSTANT && lhs[n][0].token.constant == 0.0)
				    || (lhs[n][0].kind == VARIABLE && (lhs[n][0].token.variable & VAR_MASK) > SIGN)))
				    || n_rhs[n] == 1) {
					if (solve_espace(n, cur_equation)) {
						return return_result(cur_equation);
					}
					return false;
				}
			}
			if (n_rhs[n]) {
				/* RHS expression only, set equal to zero */
				n_lhs[n] = 1;
				lhs[n][0] = zero_token;
				goto return_ok;
			}
			if (n_lhs[n] == 1 && lhs[n][0].kind == CONSTANT && fmod(lhs[n][0].token.constant, 1.0) == 0.0
			    && lhs[n][0].token.constant > 0.0 && lhs[n][0].token.constant <= n_equations) {
				/* easy selecting of equation spaces by just typing in the equation number */
				n_lhs[n] = 0;
				n = lhs[n][0].token.constant - 1;
				goto return_ok;
			}
#if	!LIBRARY
			if (autocalc) {
				/* the numerical input calculation */
				if (n_lhs[n]) {
					if (!exp_is_numeric(lhs[n], n_lhs[n])) {
						goto return_ok;		/* not numerical (contains a variable) */
					}
					/* copy the LHS to the RHS */
					blt(rhs[n], lhs[n], n_lhs[n] * sizeof(token_type));
					n_rhs[n] = n_lhs[n];
				}
				/* make the expression an equation by making the LHS the "answer" variable */
				lhs[n][0].level = 1;
				lhs[n][0].kind = VARIABLE;
				parse_var(&lhs[n][0].token.variable, "answer");
				n_lhs[n] = 1;
				i = cur_equation;
				/* temporarily make it the current equation and run the calculate command on it */
				cur_equation = n;
				calculate_cmd("");
				cur_equation = i;
				/* delete from memory */
				n_lhs[n] = 0;
				n_rhs[n] = 0;
				return true;
			}
#endif
		}
return_ok:
		cur_equation = n;
		return return_result(cur_equation);
	}
	n_lhs[n] = 0;
	n_rhs[n] = 0;
	return false;
}

/*
 * Process a line of input to Mathomatic.
 * It may be a command, an expression, an equation, etc.
 *
 * Return true if successful.
 */
int
process(cp)
char	*cp;
{
	char	*cp1;
	char	*cp_start;
	int	i, i1;
	int	len;
	int	rv;
	char	buf2[MAX_CMD_LEN];
	char	*filename;
	FILE	*fp;
	int	append_flag;

	init_gvars();		/* make sure we are in the default state */
	set_sign_array();	/* register all sign variables so that the next ones will be unique */
	if (cp == NULL) {
		return false;
	}
	cp_start = cp;
	cp = skip_space(cp);
/* handle the equation number selector */
	if (*cp == '#') {
		cp++;
		switch (*cp) {
		case '+':
		case '-':
			i = decstrtol(cp, &cp1);
			i = cur_equation + i;
			break;
		default:
			i = decstrtol(cp, &cp1) - 1;
			break;
		}
		if (cp == cp1)
			return true;	/* treat as comment */
		cp = cp1;
		if (i < 0 || i >= n_equations) {
			error(_("Equation not defined."));
			return false;
		}
		if (*cp == ':') {
			cp++;
		}
		cp = skip_space(cp);
		if (*cp) {
			input_column += (cp - cp_start);
			return parse(i, cp);
		}
		cur_equation = i;
		return return_result(cur_equation);
	}
#if	(UNIX || CYGWIN) && !SECURE && !LIBRARY
/* handle shell escape */
	if (*cp == '!') {
		cp1 = getenv("SHELL");
		if (cp1 == NULL) {
			cp1 = "/bin/sh";
		}
		if (access(cp1, X_OK)) {
			error("Shell not found or not executable, check SHELL environment variable.");
			return false;
		}
		cp = skip_space(cp + 1);
		rv = shell_out(*cp ? cp : cp1);
		return !rv;
	}
#endif
/* a quick way to get help */
	if (*cp == '?') {
		cp = skip_space(cp + 1);
		input_column += (cp - cp_start);
		return(help_cmd(cp));
	}
/* See if the string pointed to by cp is a command. */
/* If so, execute it. */
	cp1 = cp;
	while (*cp1 && !isspace(*cp1))
		cp1++;
	len = cp1 - cp;	/* length of possible command name in cp */
	for (i = 0; i < ARR_CNT(com_list); i++) {
		if ((len >= min(4, strlen(com_list[i].name)) && strncasecmp(cp, com_list[i].name, len) == 0)
		    || (com_list[i].secondary_name && len >= min(4, strlen(com_list[i].secondary_name)) && strncasecmp(cp, com_list[i].secondary_name, len) == 0)) {
			cp1 = skip_space(cp1);
			input_column += (cp1 - cp_start);
			if (my_strlcpy(buf2, cp1, sizeof(buf2)) >= sizeof(buf2)) {
				error(_("Command line too long."));
				return false;
			}
			fp = NULL;
#if	!SECURE
			/* handle output redirection */
			append_flag = false;
			filename = NULL;
			for (i1 = strlen(buf2) - 1; i1 >= 0; i1--) {
				if (buf2[i1] == '>') {
					filename = skip_space(&buf2[i1+1]);
					if (i1 && buf2[i1-1] == '>') {
						i1--;
						append_flag = true;
					}
					buf2[i1] = '\0';
					break;
				}
			}
			if (filename) {
				if (append_flag) {
					fp = fopen(filename, "a");
				} else {
					fp = fopen(filename, "w");
				}
				if (fp == NULL) {
					error(_("Can't open redirected output file for writing."));
					return false;
				}
				if (gfp != stdout) {
					fclose(gfp);	/* make sure previous redirection file is closed */
				}
				gfp = fp;
			}
#endif
			remove_trailing_spaces(buf2);
/* execute the command by calling the command function */
			rv = (*com_list[i].func)(buf2);
#if	!SECURE
			if (fp && gfp != stdout) {
				fclose(gfp);
				gfp = stdout;
			}
#endif
#if	false
/* This shouldn't display usage while reading in files; only display when the user has typed the command. */
			if (!rv && debug_level >= 0) {
				printf("Command usage: %s %s\n", com_list[i].name, com_list[i].usage);
			}
#endif
			return rv;
		}
	}
/* cp is not a command, so parse the expression */
	i = next_espace();
	input_column += (cp - cp_start);
	return process_parse(i, cp);
}

#if	(UNIX || CYGWIN) && !SECURE && !LIBRARY
/*
 * Execute a system command.  Note that system(3) requires "/bin/sh".
 *
 * Returns exit value of command (0 if no error).
 */
int
shell_out(cp)
char	*cp;	/* system command string */
{
	int	rv;

	reset_attr();
	rv = system(cp);
	printf("\n");
	if (rv < 0) {
		perror("system(3) call failed");
	}
	default_color();
	return rv;
}
#endif

/*
 * Parse a variable name with before and after space character skipping.
 *
 * Return new position in string or NULL if error.
 */
char	*
parse_var2(vp, cp)
long	*vp;	/* pointer to returned variable in Mathomatic internal format */
char	*cp;	/* pointer to variable name string */
{
	cp = skip_space(cp);
	cp = parse_var(vp, cp);
	if (cp == NULL) {
		return NULL;
	}
	return skip_space(cp);
}

#define P(A)	fprintf(gfp, "%s\n", A)

/*
 * Output command info and usage.
 *
 * Return the number of lines output.
 */
int
display_command(i)
int	i;	/* command table index of command */
{
	int	rows = 3;

	fprintf(gfp, "%s - %s\n", com_list[i].name, com_list[i].info);
	fprintf(gfp, "Usage: %s %s\n", com_list[i].name, com_list[i].usage);
	if (com_list[i].secondary_name) {
		fprintf(gfp, "Alternate name for this command: %s\n", com_list[i].secondary_name);
		rows++;
	}
	if (com_list[i].extra) {
		fprintf(gfp, "%s\n", com_list[i].extra);
		rows++;
	}
	fprintf(gfp, "\n");
	return rows;
}

/*
 * The help command.
 */
int
help_cmd(cp)
char	*cp;
{
	int	i, j;
	char	*cp1;
	int	flag;
	int	row;

	cp1 = cp;
	while (*cp1 && !isspace(*cp1))
		cp1++;
	if (cp1 != cp) {
		/* first, see if the argument matches any command names */
		flag = false;
next_argument:
		for (i = 0; i < ARR_CNT(com_list); i++) {
			if (strncasecmp(cp, com_list[i].name, cp1 - cp) == 0
			    || (com_list[i].secondary_name && strncasecmp(cp, com_list[i].secondary_name, cp1 - cp) == 0)) {
				display_command(i);
				flag = true;
			}
		}
		if (*cp == '!') {
			P("! - Shell escape, execute shell and any specified system command.");
			P("Usage: ![system-command]\n");
			flag = true;
		}
		if (flag) {
			cp1 = cp = skip_space(cp1);
			while (*cp1 && !isspace(*cp1))
				cp1++;
			if (cp1 != cp) {
				goto next_argument;
			}
			return true;
		}
		if (strncasecmp(cp, "bugs", cp1 - cp) == 0) {
			P("Report bugs to the author via email:");
			P("\tGeorge Gesslein II");
			P("\tgeorgegesslein@gmail.com");
			P("or on the Ubuntu Launchpad website:");
			P("\thttp://bugs.launchpad.net/mathomatic\n");

			P("Please include version information:");
			version_report();
			return true;
		}
		if (strncasecmp(cp, "usage", cp1 - cp) == 0) {
			P("Mathomatic Command Usage Syntax");
			P("-------------------------------");
			for (i = 0, row = 3; i < ARR_CNT(com_list); i++) {
				fprintf(gfp, "%s %s\n", com_list[i].name, com_list[i].usage);
				row++;
				if (gfp == stdout && screen_rows && row >= (screen_rows - 1)) {
					row = 1;
					if (!pause_cmd(""))
						return false;
				}
			}
			return true;
		}
		if (strncasecmp(cp, "expressions", cp1 - cp) == 0 || strncasecmp(cp, "equations", cp1 - cp) == 0) {
			P("To enter an expression or equation, simply type it in at the prompt.");
			P("Operators have precedence decreasing as indicated:\n");

			P("    - negate");
			P("    ! factorial (gamma function)");
			P("    ** or ^ power (exponentiation)");
			P("    * multiply        / divide          % modulus         // integral divide");
			P("    + add             - subtract");
			P("    = equate\n");

			P("Variables consist of any combination of letters, digits, and underscores (_).");
			P("Predefined constants and variables follow:\n");

			P("    e or e# - the universal constant e (2.7182818284...)");
			P("    pi or pi# - the universal constant pi (3.1415926535...)");
			P("    i or i# - the imaginary number (square root of -1)");
			P("The above constants may also be used most anywhere variables are required.");
			P("    sign, sign1, sign2, sign3, ... - variables that can only be +1 or -1");
			P("    integer - variable that can only be an integer value");
			P("    inf - floating point infinity constant (not a variable)\n");

			P("Absolute value notation \"|x|\" and dual polarity \"+/-x\" are understood.");
			return true;
		}
		if (is_all(cp)) {
			P("Mathomatic Command Summary");
			P("--------------------------");
			for (i = 0, row = 3; i < ARR_CNT(com_list); i++) {
				row += display_command(i);
				if (gfp == stdout && screen_rows && row >= (screen_rows - 5)) {
					row = 1;
					if (!pause_cmd(""))
						return false;
				}
			}
			P("Full documentation can be found at \"http://mathomatic.org/math/doc/\".");
			return true;
		}
		error(_("Unrecognized help topic or command."));
		return false;
	}
	/* default help text: */
	P("Mathomatic is a Computer Algebra System (CAS) and calculator program.");
	P("Type \"help bugs\" for info on how to report bugs found in this program.");
	P("Type \"help equations\" for help with entering expressions and equations.");
	P("Type \"help all\" for a summary and syntax of all commands.");
	P("Type \"help usage\" to display the syntax of all commands.");
	P("\"help\" or \"?\" followed by command names will give info on those commands.\n");

	fprintf(gfp, "These are the %d commands:\n", ARR_CNT(com_list));
	for (i = 0; i < ARR_CNT(com_list); i++) {
		if ((i % 5) == 0)
			fprintf(gfp, "\n");
		j = 15 - fprintf(gfp, "%s", com_list[i].name);
		for (; j > 0; j--)
			fprintf(gfp, " ");
	}

	P("\n\nTo select an equation space, type the equation number at the main prompt.");
	P("To solve the current equation, type the variable name at the main prompt.");
	return true;
}
