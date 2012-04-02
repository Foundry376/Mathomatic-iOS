/*
 * Mathomatic help command and parsing routines.
 *
 * Everything that depends on the command table goes here.
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

#include "includes.h"
#include "license.h"	/* the current license for Mathomatic */

#define	CMD_REQUIRED_NCHARS	4	/* Only type this many characters to run a Mathomatic command. */
					/* Set this to a high number like 50 to require all letters of a command to be typed. */

#define	REPEAT_CMD_STR	"repeat"	/* Name of the repeat command.  This command is specially handled and not listed in the table below. */

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
{	"approximate",	NULL,			approximate_cmd,	"[equation-number-ranges]",				"Approximate all numerical values in equation spaces.", "Related command: calculate" },
#if	!LIBRARY
{	"calculate",	NULL,			calculate_cmd,		"[\"factor\"] [variable number-of-iterations]",		"Temporarily plug in values for variables and approximate.", "This command may be preceded with \"repeat\"." },
#endif
{	"clear",	NULL,			clear_cmd,		"[equation-number-ranges]",				"Delete expressions stored in memory so equation spaces can be reused.", "Tip: Use \"clear all\" to quickly restart Mathomatic." },
{	"code",		NULL,			code_cmd,		"[\"c\" or \"java\" or \"python\" or \"integer\"] [equation-number-ranges]",	"Output C, Java, or Python code for the specified equations.", "Related commands: simplify, optimize, and variables" },
{	"compare",	NULL,			compare_cmd,		"[\"symbolic\"] equation-number [\"with\" equation-number]",	"Compare two equation spaces to see if mathematically the same." },
{	"copy",		NULL,			copy_cmd,		"[equation-number-range]",				"Duplicate the contents of the specified equation spaces." },
{	"derivative",	"differentiate",	derivative_cmd,		"[\"nosimplify\"] [variable or \"all\"] [order]",	"Symbolically differentiate and simplify, order times." },
{	"display",	NULL,			display_cmd,		"[\"factor\"] [equation-number-ranges]",			"Display equation spaces in pretty multi-line (2D) fraction format." },
{	"divide",	NULL,			divide_cmd,		"[variable]",						"Prompt for 2 numbers or polynomials and divide. Give result and GCD.", "This command may be preceded with \"repeat\"." },
{	"echo",		NULL,			echo_cmd,		"[text]",						"Output a line of text, followed by a newline." },
#if	(UNIX || CYGWIN) && !SECURE && !LIBRARY
{	"edit",		NULL,			edit_cmd,		"[file-name]",						"Edit all equation spaces or an input file, then read them in." },
#endif
{	"eliminate",	NULL,			eliminate_cmd,		"variables or \"all\" [\"using\" equation-number]",	"Substitute the specified variables with solved equations.", "This command may be preceded with \"repeat\"." },
{	"extrema",	NULL,			extrema_cmd,		"[variable] [order]",					"Show where the slope of the current equation equals zero.", "Helps with finding the minimums and maximums." },
{	"factor",	"collect",		factor_cmd,		"[\"number\" [integers]] or [\"power\"] [equation-number-range] [variables]",	"Factor variables in equation spaces or factor given integers." },
{	"fraction",	"together",		fraction_cmd,		"[equation-number-range]",				"Convert expression to a single simple fraction." },
{	"help",		NULL,			help_cmd,		"[topics or command-names]",				"Short, built-in help and reference." },
{	"imaginary",	NULL,			imaginary_cmd,		"[variable]",						"Copy the imaginary part of the current expression.", "Related command: real" },
{	"integrate",	"integral",		integrate_cmd,		"[\"constant\" or \"definite\"] variable [order]",	"Symbolically integrate polynomials order times, then simplify." },
{	"laplace",	NULL,			laplace_cmd,		"[\"inverse\"] variable",				"Compute the Laplace or inverse Laplace transform of polynomials.", "This command only works with polynomials." },
{	"limit",	NULL,			limit_cmd,		"variable expression",					"Take the limit as variable goes to expression (experimental).", "This command is experimental." },
{	"list",		NULL,			list_cmd,		"[\"export\" or \"maxima\" or \"gnuplot\" or \"hexadecimal\"] [equation-number-ranges]",	"Display equation spaces in single-line format.", "Options to export to other math programs." },
{	"nintegrate",	NULL,			nintegrate_cmd,		"[\"trapezoid\"] variable [partitions]",		"Do numerical definite integration using Simpson's rule.", "This command cannot integrate over singularities." },
{	"optimize",	NULL,			optimize_cmd,		"[equation-number-range]",				"Split up equations into smaller, more efficient equations.", "Related command: code" },
{	"pause",	NULL,			pause_cmd,		"[text]",						"Wait for user to press the Enter key. Optionally display a message." },
#if	(UNIX || CYGWIN) && !SECURE && !LIBRARY
{	"plot",		NULL,			plot_cmd,		"[x-range [y-range]] [expression,]",			"Automatically plot expression in 2D or 3D with Gnuplot.", "Plots variable x, and y if 3D surface plot." },
#endif
{	"product",	NULL,			product_cmd,		"variable start end [step-size]",			"Compute the product as variable goes from start to end.", "Related command: sum" },
#if	READLINE
{	"push",		NULL,			push_cmd,		"[equation-number-range]",				"Push equation spaces into readline history for editing." },
#endif
#if	!LIBRARY
{	"quit",		"exit",			quit_cmd,		"[exit-value]",						"Terminate this program without saving." },
#endif
#if	!SECURE
{	"read",		NULL,			read_cmd,		"file-name",						"Read in a text file as if it was typed in.", "Related command: save" },
#endif
{	"real",		NULL,			real_cmd,		"[variable]",						"Copy the real part of the current expression.", "Related command: imaginary" },
{	"replace",	NULL,			replace_cmd,		"[variables [\"with\" expression]]",			"Substitute variables in the current equation with expressions." },
{	"roots",	NULL,			roots_cmd,		"root real-part imaginary-part",			"Display all the roots of a complex number.", "This command may be preceded with \"repeat\"." },
#if	!SECURE
{	"save",		NULL,			save_cmd,		"file-name",						"Save all equation spaces in a text file.", "Related command: read" },
#endif
{	"set",		NULL,			set_cmd,		"[[\"no\"] option] ...",				"Display, set, or save current session options.", "\"set\" by itself will show all current option settings." },
{	"simplify",	NULL,			simplify_cmd,		"[\"sign\"] [\"symbolic\"] [\"quick[est]\"] [\"fraction\"] [equation-number-range]",	"Completely simplify expressions.", "This command may be preceded with \"repeat\"." },
{	"solve",	NULL,			solve_cmd,		"[\"verify\"] [\"for\"] variable or \"0\"",		"Solve the current equation for a variable or for zero.", "The verify option checks all displayed solutions." },
{	"sum",		NULL,			sum_cmd,		"variable start end [step-size]",			"Compute the summation as variable goes from start to end.", "Related command: product" },
#if	!LIBRARY
{	"tally",	NULL,			tally_cmd,		"[\"average\"]",					"Prompt for and add entries, show total and optionally the average." },
#endif
{	"taylor",	NULL,			taylor_cmd,		"[\"nosimplify\"] variable order point",		"Compute the Taylor series expansion of the current expression." },
{	"unfactor",	"expand",		unfactor_cmd,		"[\"fraction\"] [\"quick\"] [\"power\"] [equation-number-range]",	"Algebraically expand (multiply out) expressions." },
{	"variables",	NULL,			variables_cmd,		"[\"c\" or \"java\" or \"integer\"] [equation-number-range]",	"Show all variable names used within the specified expressions.", "Related command: code" },
{	"version",	NULL,			version_cmd,		"",							"Display Mathomatic version and license information." }
};

#if	!LIBRARY
char	*example_strings[] = {
	"; The derivative of the absolute value function:\n",
	"|x|\n",
	"derivative\n",
	"pause\n",
	"\n",
	"; Enter a common temperature conversion formula:\n",
	"fahrenheit = (9*celsius/5) + 32\n",
	"repeat calculate ; plug in values until an empty line is entered\n",
	"; Solve for the other variable and simplify the result:\n",
	"solve for celsius\n",
	"simplify\n",
	"variables ; show all variables that occur in expressions\n",
	"pause\n",
	"\n",
	"; Expand the following into polynomial form, then refactor:\n",
	"(x+1)^5\n",
	"unfactor\n",
	"simplify\n",
	NULL
};
#endif

/*
 * Process mathematical expression input in Mathomatic,
 * with no solving and no automatic calculation.
 *
 * Simply parse the equation or expression text in "cp"
 * and place the result in equation space "n".
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
	return false;
}

/*
 * Process equation and expression input in Mathomatic,
 * either swapping equation sides, selecting an equation space (autoselect),
 * solving the current equation (autosolve), calculating a numerical expression (autocalc),
 * or storing.
 *
 * Parse the mathematical expression text in "cp" and perform one of the above operations,
 * or store it in equation space "n" and display.
 *
 * Return true if successful.
 */
int
process_parse(n, cp)
int	n;
char	*cp;
{
	int	i;
	char	*cp1, *ep;
	int	equals_flag = false;

	if ((ep = parse_equation(n, cp))) {
		for (cp1 = cp; cp1 < ep; cp1++) {
			if (*cp1 == '=') {
				equals_flag = true;
				break;
			}
		}
		if (n_lhs[n] == 0 && n_rhs[n] == 0) {
			if (strcmp(cp, "=") == 0 && n_lhs[cur_equation] > 0 && n_rhs[cur_equation] > 0) {
				debug_string(0, _("Swapping both sides of the current equation..."));
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
			if (autosolve || equals_flag) {
				if ((n_lhs[n] == 1 && ((lhs[n][0].kind == CONSTANT && lhs[n][0].token.constant == 0.0)
				    || (lhs[n][0].kind == VARIABLE && ((lhs[n][0].token.variable & VAR_MASK) > SIGN || equals_flag))))
				    || (n_rhs[n] == 1 && ((rhs[n][0].kind == CONSTANT && rhs[n][0].token.constant == 0.0)
				    || rhs[n][0].kind == VARIABLE))) {
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
			if (autoselect && n_lhs[n] == 1 && lhs[n][0].kind == CONSTANT && fmod(lhs[n][0].token.constant, 1.0) == 0.0
			    && lhs[n][0].token.constant > 0.0 && lhs[n][0].token.constant <= n_equations) {
				/* easy selecting of equation spaces by just typing in the equation number */
				cur_equation = lhs[n][0].token.constant - 1;
				n_lhs[n] = 0;
				return_result(cur_equation);
				return true;
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
	return false;
}

/*
 * Run a line of main prompt input to Mathomatic.
 * It may be a command, an expression or equation to store, etc.
 *
 * Return true if successful.
 */
int
process(cp)
char	*cp;
{
	char	*cp1;
	char	*cp_start;
	int	i;
	int	len;
	int	rv;
	char	buf2[MAX_CMD_LEN];
	int	our_repeat_flag = false;
#if	!SECURE
	FILE	*fp;
	char	*filename;
	int	append_flag;
	int	i1;
#endif

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
		if (!alloc_to_espace(i)) {
			error(_("Equation number out of range."));
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
		return_result(cur_equation);
		return true;
	}
#if	(UNIX || CYGWIN) && !SECURE && !LIBRARY
/* handle shell escape */
	if (security_level == 0 && *cp == '!') {
		cp1 = getenv("SHELL");
		if (cp1 == NULL) {
			cp1 = "/bin/sh";
		}
		if (access(cp1, X_OK)) {
			error(_("Shell not found or not executable, check SHELL environment variable."));
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
do_repeat:
	cp1 = cp;
	while (*cp1 && !isspace(*cp1))
		cp1++;
	len = cp1 - cp;	/* length of possible command name in cp */
	if (len >= min(CMD_REQUIRED_NCHARS, strlen(REPEAT_CMD_STR)) && len <= strlen(REPEAT_CMD_STR) && strncasecmp(cp, REPEAT_CMD_STR, len) == 0) {
		our_repeat_flag = true;
		cp = skip_space(cp1);
		goto do_repeat;
	}
	for (i = 0; i < ARR_CNT(com_list); i++) {
		if ((len >= min(CMD_REQUIRED_NCHARS, strlen(com_list[i].name)) && len <= strlen(com_list[i].name) && strncasecmp(cp, com_list[i].name, len) == 0)
		    || (com_list[i].secondary_name && len >= min(CMD_REQUIRED_NCHARS, strlen(com_list[i].secondary_name)) && len <= strlen(com_list[i].secondary_name) && strncasecmp(cp, com_list[i].secondary_name, len) == 0)) {
			cp1 = skip_space(cp1);
			input_column += (cp1 - cp_start);
/* Copy the command line to buf2 and use it, because the original string may be overwritten. */
			if (my_strlcpy(buf2, cp1, sizeof(buf2)) >= sizeof(buf2)) {
				error(_("Command line too long."));
				return false;
			}
#if	!SECURE
			fp = NULL;
			if (security_level < 2) {
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
						perror(filename);
						error(_("Can't open redirected output file for writing."));
						return false;
					}
					if (gfp != stdout && gfp != stderr && gfp != default_out) {
						fclose(gfp);	/* make sure previous redirection file is closed */
					}
					gfp = fp;
				}
			}
#endif
			remove_trailing_spaces(buf2);
			pull_number = 1;
/* execute the command by calling the command function */
			repeat_flag = our_repeat_flag;
			rv = (*com_list[i].func)(buf2);
			repeat_flag = false;
#if	!SECURE
			if (fp && gfp != default_out) {
				if (gfp != stdout && gfp != stderr)
					fclose(gfp);
				gfp = default_out;
			}
#endif
#if	!SILENT
			if (!rv && debug_level >= 0) {
				printf("Command usage: %s %s\n", com_list[i].name, com_list[i].usage);
			}
#endif
			return rv;
		}
	}
	if (our_repeat_flag) {
		error(_("Follow with command to automatically repeat."));
		return false;
	}
/* cp is not a command, so parse the expression */
	i = next_espace();
	input_column += (cp - cp_start);
	return process_parse(i, cp);
}

/*
 * Display and process Mathomatic main prompt input.
 * Used by the read command and "help examples".
 * The input string will be shortened by set_error_level().
 *
 * Return true if successful.
 */
int
display_process(cp)
char	*cp;	/* string to process, will be modified */
{
	int	len;
	int	nlt;	/* true if cp is newline terminated */

	if (cp == NULL)
		return false;
	len = strlen(cp);
	if (len > 0)
		len--;
	nlt = (cp[len] == '\n');
	if (!quiet_mode) {
#if	LIBRARY
		fprintf(gfp, "%d%s%s", cur_equation + 1, PROMPT_STR, cp);
#else
		default_color();
		input_column = printf("%d%s", cur_equation + 1, html_flag ? HTML_PROMPT_STR : PROMPT_STR);
		if (html_flag) {
			input_column -= (strlen(HTML_PROMPT_STR) - strlen(PROMPT_STR));
			printf("<b>%s</b>", cp);	/* make input bold */
		} else {
			printf("%s", cp);
		}
		if (!nlt)
			printf("\n");
		if (gfp != stdout && gfp != stderr) {
			fprintf(gfp, "%d%s%s", cur_equation + 1, PROMPT_STR, cp);
			if (!nlt)
				fprintf(gfp, "\n");
		}
#endif
	}
	set_error_level(cp);
	return process(cp);
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

	if (security_level) {
		error_bug("Shelling out disabled.");
	}
	reset_attr();

	errno = 0;
	rv = system(cp);
	if (rv < 0) {
		perror("system(3) call failed");
	}

	printf("\n");
	default_color();
	return rv;
}
#endif

/*
 * Parse a variable name with before and after space character skipping.
 *
 * Return new position in string or NULL if error.
 */
char *
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

#if	!LIBRARY
int
read_examples(cpp)
char	**cpp;
{
	int	i;
	char	*cp;

	for (i = 0; cpp[i]; i++) {
		cp = strdup(cpp[i]);
		if (cp == NULL)
			return false;
		if (!display_process(cp)) {
			free(cp);
			return false;
		}
		free(cp);
	}
	return true;
}
#endif

/*
 * Display a row of dashes to underline a title.
 */
void
underline_title(count)
int	count;
{
	int	i;

	for (i = 1; i < count; i++) {
		fprintf(gfp, "-");
	}
	fprintf(gfp, "\n");
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

	if (*cp == '\0') {
		/* default help text: */
		P("Mathomatic is a Computer Algebra System (CAS) and calculator program.");
		P("To see helpful interactive examples, type \"help examples\".");
		P("Type \"help equations\" for help with entering expressions and equations.");
		P("Type \"help all\" for a summary and syntax of all commands.");
		P("Type \"help usage\" to display the syntax of all commands.");
		P("Other help keywords: constants, geometry, license, bugs, options.");
		P("\"help\" or \"?\" followed by a command name will give info on that command.");
		fprintf(gfp, "These are the %d commands for this version of Mathomatic:\n", ARR_CNT(com_list));
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

next_argument:
	cp1 = cp;
	while (*cp1 && !isspace(*cp1))
		cp1++;
	if (cp1 != cp) {
		P("******************************************************************************");
		/* first, see if the argument matches any command names */
		flag = false;
		for (i = 0; i < ARR_CNT(com_list); i++) {
			if (strncasecmp(cp, com_list[i].name, cp1 - cp) == 0
			    || (com_list[i].secondary_name && strncasecmp(cp, com_list[i].secondary_name, cp1 - cp) == 0)) {
				display_command(i);
				flag = true;
			}
		}
		if (flag) {
			cp = skip_space(cp1);
			goto next_argument;
		}
#if	!LIBRARY
		if (strncasecmp(cp, "options", cp1 - cp) == 0) {
			usage(gfp);
			cp = skip_space(cp1);
			goto next_argument;
		}
		if (strncasecmp(cp, "examples", cp1 - cp) == 0) {
			return read_examples(example_strings);
		}
#endif
		if (strncasecmp(cp, "copyright", cp1 - cp) == 0 || strncasecmp(cp, "license", cp1 - cp) == 0) {
			underline_title(P("Copyright and License for Mathomatic"));

			fprintf(gfp, "%s", license_string);
			cp = skip_space(cp1);
			goto next_argument;
		}
		if (strncasecmp(cp, "bugs", cp1 - cp) == 0) {
			P("Please report bugs on the Mathomatic project site on Launchpad.net:");
			P("https://launchpad.net/mathomatic\n");

			P("Launchpad features a complete bug management system.");
			P("Please include the following information (from the version command):\n");

			version_report();
			cp = skip_space(cp1);
			goto next_argument;
		}
		if (strncasecmp(cp, "usage", cp1 - cp) == 0) {
			underline_title(P("Mathomatic Command Usage Syntax"));
			for (i = 0, row = 3; i < ARR_CNT(com_list);) {
				fprintf(gfp, "%s %s\n", com_list[i].name, com_list[i].usage);
				row++;
				i++;
				if (i >= ARR_CNT(com_list))
					break;
				if (screen_rows && row >= (screen_rows - 3)) {
					row = 2;
					if (gfp == stdout) {
						if (!pause_cmd(""))
							return false;
					}
				}
			}
			cp = skip_space(cp1);
			goto next_argument;
		}
		if (strncasecmp(cp, "geometry", cp1 - cp) == 0) {
			underline_title(P("Commonly used standard (Euclidean) geometric formulas"));
			P("Triangle of base \"b\" and height \"h\":");
			P("    area = b*h/2\n");

			P("Rectangle of length \"l\" and width \"w\":");
			P("    area = l*w                    perimeter = 2*l + 2*w\n");

			P("Trapezoid of parallel sides \"a\" and \"b\", and \"d\" distance between them:");
			P("    area = d*(a + b)/2\n");

			P("Circle of radius \"r\":");
			P("    area = pi*r^2                 perimeter = 2*pi*r\n");

			P("Rectangular solid of length \"l\", width \"w\", and height \"h\":");
			P("    volume = l*w*h                surface_area = 2*l*w + 2*l*h + 2*w*h\n");

			P("Sphere of radius \"r\":");
			P("    volume = 4/3*pi*r^3           surface_area = 4*pi*r^2\n");

			P("Convex polygon of \"n\" sides, sum of all interior angles formula:");
			P("    sum = (n - 2)*180 degrees     sum = (n - 2)*pi radians");
			cp = skip_space(cp1);
			goto next_argument;
		}
		if (strncasecmp(cp, "expressions", cp1 - cp) == 0 || strncasecmp(cp, "equations", cp1 - cp) == 0) {
			P("To enter an expression or equation, simply type it in at the prompt.");
			P("Operators have precedence decreasing as indicated:\n");

			P("    ! factorial (gamma function)");
			P("    ** or ^ power (exponentiation)");
			P("    * multiply      / divide        % modulus       // integral divide");
			P("    + add           - subtract");
			P("    = equate (lowest precedence)\n");

			P("Multiple operators of the same precedence level are grouped left to right.\n");
			P("Variables consist of any combination of letters, digits, and underscores (_).");
			P("Predefined variables follow:\n");

			P("    sign, sign1, sign2, ... - may only be +1 or -1");
			P("    integer, integer1, ... - may be any integer value\n");

			P("Absolute value notation \"|x|\" and dual polarity \"+/-x\" are understood.");
			cp = skip_space(cp1);
			goto next_argument;
		}
		if (strncasecmp(cp, "constants", cp1 - cp) == 0) {
			P("Constants are double precision floating point values with about");
			P("14 decimal digits accuracy.  They can be entered in standard, scientific,");
			P("or hexadecimal notation.  Excepting named constants, constants always");
			P("start with a decimal digit (0..9) or a period.\n");

			P("Named constants follow:\n");

			P("    e or e# - the universal constant e (2.7182818284...)");
			P("    pi or pi# - the universal constant pi (3.1415926535...)");
			P("    i or i# - the imaginary unit (square root of -1)");
			P("The above constants may also be used anywhere variables are required.");
			P("    inf - floating point infinity constant");
			P("    nan - invalid floating point result (not enterable)\n");

			fprintf(gfp, "The largest value of a constant is +/-%g = +/-2^1024.\n", DBL_MAX);
			fprintf(gfp, "The smallest value of a constant is +/-%g or 0.\n", DBL_MIN);
			cp = skip_space(cp1);
			goto next_argument;
		}
		if (strncasecmp(cp, "table", cp1 - cp) == 0) {
			fprintf(gfp, "Command\tUsage\tNotes\n");
			for (i = 0; i < ARR_CNT(com_list); i++) {
				fprintf(gfp, "%s", com_list[i].name);
				fprintf(gfp, "\t%s %s", com_list[i].name, com_list[i].usage);
				if (com_list[i].secondary_name) {
					fprintf(gfp, "\tAlternate name for this command: %s", com_list[i].secondary_name);
				}
				if (com_list[i].extra) {
					fprintf(gfp, "\t%s", com_list[i].extra);
				}
				fprintf(gfp, "\n");
			}
			cp = skip_space(cp1);
			goto next_argument;
		}
		if (is_all(cp)) {
			underline_title(fprintf(gfp, "Mathomatic Version %s Command Summary\n", VERSION));
			for (i = 0, row = 3; i < ARR_CNT(com_list);) {
				row += display_command(i);
				i++;
				if (i >= ARR_CNT(com_list))
					break;
				if (screen_rows && row >= (screen_rows - 5)) {
					row = 1;
					if (gfp == stdout) {
						if (!pause_cmd(""))
							return false;
					}
				}
			}
			fprintf(gfp, "End of command list.  Total of %d different commands.\n", ARR_CNT(com_list));
			cp = skip_space(cp1);
			goto next_argument;
		}
		error(_("Unrecognized help topic or command."));
		return false;
	}
	return true;
}
