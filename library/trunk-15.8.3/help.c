/*
 * Mathomatic help command and parsing routines.
 *
 * Everything that depends on the command table goes here.
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

#include "includes.h"
#include "license.h"	/* the current software license for Mathomatic */

/*
 * The following are macros for displaying help text.
 * When used properly, they will allow nicer looking wrap-around on all help text.
 * If NOT80COLUMNS is 1, paragraphs will all be one long line.
 * This is helpful when the output device has less than 80 text character columns.
 */
#if	NOT80COLUMNS
#define SP(A)	fprintf(gfp, "%s ", A)	/* display part of a paragraph, separated with spaces (Space Paragraph) */
#define EP(A)	fprintf(gfp, "%s\n", A)	/* display the end of a paragraph (End Paragraph) */
#else	/* Otherwise the following only works with 80 column or wider screens */
#define SP(A)	fprintf(gfp, "%s\n", A)
#define EP(A)	fprintf(gfp, "%s\n", A)
#endif

#define	CMD_REQUIRED_NCHARS	4	/* Only type this many characters to run a Mathomatic command. */
					/* Set this to a high number like 50 to require all letters of a command to be typed. */

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
{	"calculate",	NULL,			calculate_cmd,		"[\"factor\"] [equation-number-range] [variable iterations]",	"Temporarily plug in values for variables and approximate.", "This command may be preceded with \"repeat\"." },
#endif
{	"clear",	NULL,			clear_cmd,		"[equation-number-ranges]",				"Delete expressions stored in memory so equation spaces can be reused.", "Tip: Use \"clear all\" to quickly restart Mathomatic." },
{	"code",		NULL,			code_cmd,		"[\"c\" or \"java\" or \"python\" or \"integer\"] [equation-number-ranges]",	"Output C, Java, or Python code for the specified equations.", "Related commands: simplify, optimize, and variables" },
{	"compare",	NULL,			compare_cmd,		"[\"symbolic\" \"approx\"] equation-number [\"with\" equation-number]",	"Compare two equation spaces for mathematical equivalence.", "This command may be preceded with \"repeat\"." },
{	"copy",		NULL,			copy_cmd,		"[equation-number-ranges]",				"Duplicate the contents of the specified equation spaces." },
{	"derivative",	"differentiate",	derivative_cmd,		"[\"nosimplify\"] variable or \"all\" [order]",		"Symbolically differentiate and simplify, order times." },
{	"display",	NULL,			display_cmd,		"[\"factor\" \"mixed\"] [equation-number-ranges]",	"Display equation spaces in pretty, 2D multi-line fraction format." },
{	"divide",	NULL,			divide_cmd,		"[base-variable] [dividend divisor]",			"Divide 2 numbers or polynomials. Give detailed result and GCD.", "This command may be preceded with \"repeat\"." },
{	"echo",		NULL,			echo_cmd,		"[text]",						"Output a line of text, followed by a newline.", "This command may be preceded with \"repeat\"." },
#if	SHELL_OUT
{	"edit",		NULL,			edit_cmd,		"[file-name]",						"Edit all equation spaces or an input file, then read them in.", "Editor name in EDITOR environment variable." },
#endif
{	"eliminate",	NULL,			eliminate_cmd,		"variables or \"all\" [\"using\" equation-number]",	"Substitute the specified variables with solved equations.", "This command may be preceded with \"repeat\"." },
{	"extrema",	NULL,			extrema_cmd,		"[variable] [order]",					"Show where the slope of the current equation equals zero.", "Helps with finding the minimums and maximums." },
{	"factor",	"collect",		factor_cmd,		"[\"number\" [integers]] or [\"power\"] [equation-number-range] [variables]",	"Factor variables in equation spaces or factorize given integers." },
{	"for",		NULL,			for_cmd,		"variable start end [step-size]",			"Evaluate and display the current expression for each value of variable.", "Same syntax as the sum and product commands." },
{	"fraction",	NULL,			fraction_cmd,		"[\"numerator\" \"denominator\"] [equation-number-range]",	"Convert expression to a single simple algebraic fraction.", "This command may be preceded with \"repeat\"." },
#if	HELP
{	"help",		"?",			help_cmd,		"[topics or command-names]",				"Short, built-in help and reference." },
#endif
{	"imaginary",	NULL,			imaginary_cmd,		"[variable]",						"Copy the imaginary part of the current expression.", "Related command: real" },
{	"integrate",	"integral",		integrate_cmd,		"[\"constant\" or \"definite\"] variable [order]",	"Symbolically integrate polynomials order times, then simplify." },
{	"laplace",	NULL,			laplace_cmd,		"[\"inverse\"] variable",				"Compute the Laplace or inverse Laplace transform of polynomials.", "This command only works with polynomials." },
{	"limit",	NULL,			limit_cmd,		"variable expression",					"Take the limit as variable goes to expression.", "This limit command is experimental." },
{	"list",		NULL,			list_cmd,		"[\"export\" or \"maxima\" or \"gnuplot\" or \"hex\"] [equation-number-ranges]",	"Display equation spaces in single-line (one-dimensional) format.", "Options to export expressions to other math programs." },
{	"nintegrate",	NULL,			nintegrate_cmd,		"[\"trapezoid\"] variable [partitions [lower-bound upper-bound]]",	"Do numerical definite integration using Simpson's rule.", "This command cannot integrate over singularities." },
{	"optimize",	NULL,			optimize_cmd,		"[equation-number-range]",				"Split up equations into smaller, more efficient equations." },
{	"pause",	NULL,			pause_cmd,		"[text]",						"Display a line of text and wait for user to press the Enter key." },
#if	SHELL_OUT
{	"plot",		NULL,			plot_cmd,		"[equation-number-ranges] [xyz-ranges] [expressions,]",	"Automatically plot expressions in 2D or 3D with gnuplot.", "Plots variable x; if expression contains y, do a 3D surface plot." },
#endif
{	"product",	NULL,			product_cmd,		"variable start end [step-size]",			"Compute the product as variable goes from start to end.", "Related command: sum" },
#if	READLINE
{	"push",		NULL,			push_cmd,		"[equation-number-ranges or text-to-push]",		"Push equation spaces or text into readline history for editing.", "Available only if readline is enabled." },
#endif
#if	!LIBRARY
{	"quit",		"exit",			quit_cmd,		"[exit-value]",						"Terminate this program without saving." },
#endif
#if	!SECURE
{	"read",		NULL,			read_cmd,		"file-name",						"Read in a text file as if it was typed in.", "This command may be preceded with \"repeat\"." },
#endif
{	"real",		NULL,			real_cmd,		"[variable]",						"Copy the real part of the current expression.", "Related command: imaginary" },
{	"replace",	NULL,			replace_cmd,		"[variables [\"with\" expression]]",			"Substitute variables in the current equation with expressions." },
{	"roots",	NULL,			roots_cmd,		"root real-part imaginary-part",			"Display all the roots of a complex number.", "This command may be preceded with \"repeat\"." },
#if	!SECURE
{	"save",		NULL,			save_cmd,		"file-name",						"Save all equation spaces in a text file.", "Related command: read" },
#endif
{	"set",		NULL,			set_cmd,		"[[\"no\"] option [value]] ...",			"Display, set, or save current session options.", "\"set\" by itself will show all current option settings." },
{	"simplify",	NULL,			simplify_cmd,		"[\"sign\" \"symbolic\" \"quick[est]\" \"fraction\"] [equation-number-ranges]",	"Completely simplify expressions.", "This command may be preceded with \"repeat\"." },
{	"solve",	NULL,			solve_cmd,		"[\"verify\"] [equation-number-range] [\"for\"] variable or \"0\"",	"Solve the specified equations for a variable or for zero.", "The verify option checks all displayed solutions." },
{	"sum",		NULL,			sum_cmd,		"variable start end [step-size]",			"Compute the summation as variable goes from start to end.", "Related command: product" },
#if	!LIBRARY
{	"tally",	NULL,			tally_cmd,		"[\"average\"]",					"Prompt for and add entries, showing running total." },
#endif
{	"taylor",	NULL,			taylor_cmd,		"[\"nosimplify\"] variable order point",		"Compute the Taylor series expansion of the current expression." },
{	"unfactor",	"expand",		unfactor_cmd,		"[\"count\" \"fraction\" \"quick\" \"power\"] [equation-number-range]",	"Algebraically expand (multiply out) expressions." },
{	"variables",	NULL,			variables_cmd,		"[\"c\" or \"java\" or \"integer\" or \"count\"] [equation-number-ranges]",	"Show all variable names used within the specified expressions.", "Related command: code" },
{	"version",	NULL,			version_cmd,		"",							"Display Mathomatic version, compiler, and license information." }
};

#if	HELP && !LIBRARY
char	*example_strings[] = {
	"; Example 1:\n",
	"; Here the derivative of the absolute value function is computed.\n",
	"; Expressions are entered by just typing them in:\n",
	"|x| ; The absolute value of x\n",
	"derivative ; The result gives the sign of x:\n",
	"pause\n",
	"repeat echo -\n",
	"; Example 2:\n",
	"; Here the calculate command is used to plug values into a solved formula.\n",
	"; A common temperature conversion formula (from \"help conversions\"):\n",
	"fahrenheit = (9*celsius/5) + 32\n",
	"repeat calculate ; plug in values until an empty line is entered\n",
	"\n",
	"; Solve for the other variable and simplify the result:\n",
	"solve for celsius\n",
	"simplify\n",
	"repeat calculate ; plug in values until an empty line is entered\n",
	"\n",
	"variables ; show all variables that occur in expressions\n",
	"pause\n",
	"repeat echo -\n",
	"; Example 3:\n",
	"; Expand the following to polynomial form, then refactor and differentiate:\n",
	"(x-y)^6\n",
	"expand count ; Expand and count the resulting number of terms:\n",
	"simplify\n",
	"derivative x\n",
	NULL
};
#endif

#if	HELP
char	*geometry_strings[] = {
	"; Triangle area:\n",
	"triangle_area = base*height/2\n",
	"; Here is Heron's formula for the area of any triangle\n",
	"; given all three side lengths (a, b, and c):\n",
	"triangle_area = (((a + b + c)*(a - b + c)*(a + b - c)*(b - a + c))^(1/2))/4\n",
	"\n",
	"; Rectangle of length \"l\" and width \"w\":\n",
	"rectangle_area = l*w\n",
	"rectangle_perimeter = 2*l + 2*w\n",
	"\n",
	"; Trapezoid of parallel sides \"a\" and \"b\",\n",
	"; and the \"distance\" between them:\n",
	"trapezoid_area = distance*(a + b)/2\n",
	"\n",
	"; Circle of radius \"r\":\n",
	"circle_area = pi*r^2\n",
	"circle_perimeter = 2*pi*r\n",
	"\n",
	"; 3D rectangular solid of length \"l\", width \"w\", and height \"h\":\n",
	"brick_volume = l*w*h\n",
	"brick_surface_area = 2*l*w + 2*l*h + 2*w*h\n",
	"\n",
	"; 3D sphere of radius \"r\":\n",
	"sphere_volume = 4/3*pi*r^3\n",
	"sphere_surface_area = 4*pi*r^2\n",
	"\n",
	"; Convex 2D polygon with straight sides,\n",
	"; sum of all interior angles formula:\n",
	"sum_degrees = (sides - 2)*180\n",
	"sum_radians = (sides - 2)*pi\n",
	"; \"sides\" is the number of sides of any convex 2D polygon.\n",
	"; Convex means that all interior angles are less than 180 degrees.\n",
	"; Type \"eliminate sides\" to get radians/degrees conversion formula.\n",
	NULL
};

char	*conversion_strings[] = {
	"; Temperature\n",
	"fahrenheit = (9*celsius/5) + 32\n",
	"kelvin = celsius + 273.15\n",
	"; Distance\n",
	"inches = centimeters/2.54\n",
	"miles = kilometers/1.609344\n",
	"; Weight\n",
	"pounds = kilograms/0.45359237\n",
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
	int		i;
	char		*cp1, *ep;
	int		equals_flag = false;
	int		rv;
#if	!LIBRARY
	static int	last_autocalc_en = -1;
	long		answer_v = 0;		/* Mathomatic answer variable */
#endif

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
					rv = solve_espace(n, cur_equation);
					n_lhs[n] = 0;
					n_rhs[n] = 0;
					if (rv) {
						return return_result(cur_equation);
					} else {
						return false;
					}
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
				parse_var(&answer_v, "answer");	/* convert to a Mathomatic variable */
				lhs[n][0].token.variable = answer_v;
				n_lhs[n] = 1;
				/* make it the current equation and run the calculate command on it */
				cur_equation = n;
				rv = calculate_cmd("");	/* display the approximation */
				/* Keep the current input until next autocalc, then delete if "set autodelete". */
				i = last_autocalc_en;
				if (autodelete && i >= 0 && i < n_equations) {
					if (i != n && n_lhs[i] == 1 && lhs[i][0].kind == VARIABLE
					    && lhs[i][0].token.variable == answer_v) {
						/* delete previous answer from memory */
						n_lhs[i] = 0;
						n_rhs[i] = 0;
					}
				}
				last_autocalc_en = n;
				return rv;
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
 * Return true if line starts with a colon (:) or if successful.
 */
int
process(cp)
char	*cp;
{
	int	rv;

	if (cp && cp[0] == ':') {
		input_column++;
		process_rv(cp + 1);
		return true;
	} else {
		rv = process_rv(cp);
		if (!rv) {
			debug_string(1, "Error return.");
		}
		return rv;
	}
}

/*
 * Run a line of main prompt input to Mathomatic.
 * It may be a command, an expression or equation to store, etc.
 *
 * Return true if successful.
 */
int
process_rv(cp)
char	*cp;
{
	char	*cp1 = NULL;
	char	*cp_start;
	int	i;
	int	len;
	int	rv;
	char	buf2[MAX_CMD_LEN];	/* do not make this static! */
	int	our_repeat_flag = false;
	long	v;			/* Mathomatic variable */
#if	!SECURE
	FILE	*fp;
	int	append_flag;
	int	i1;
#endif

#if	DEBUG
	check_gvars();
#endif
	init_gvars();		/* make sure we are in the default state */
	set_sign_array();	/* register all sign variables so that the next ones will be unique */
	if (cp == NULL) {
		return false;
	}
	cp_start = cp;
	cp = skip_space(cp);
/* handle search forward */
	if (*cp == '/' && isvarchar(cp[1])) {
		cp++;
		debug_string(0, ("Searching forwards for variable."));
		if ((cp1 = parse_var(&v, cp)) == NULL) {
			return false;
		}
		if (extra_characters(cp1)) {
			return false;
		}
		if (search_all_for_var(v, true)) {
			return return_result(cur_equation);
		} else {
			error(_("Variable not found in any equation space."));
			return false;
		}
	}
/* handle the equation selector */
	if (*cp == '#') {
		cp++;
		if (isvarchar(*cp)) {
			debug_string(0, ("Searching backwards for variable."));
			if ((cp1 = parse_var(&v, cp)) == NULL) {
				return false;
			}
			if (extra_characters(cp1)) {
				return false;
			}
			if (search_all_for_var(v, false)) {
				return return_result(cur_equation);
			} else {
				error(_("Variable not found in any equation space."));
				return false;
			}
		}
		i = cur_equation;
		cp1 = cp;
		switch (*cp) {
		case '+':
		case '-':
			i += strtol(cp, &cp1, 10);
			break;
		default:
			if (isdigit(*cp)) {
				i = strtol(cp, &cp1, 10) - 1;
			}
			break;
		}
		if (cp1 == NULL || cp == cp1) {
			return true;	/* treat as comment */
		}
		if (*cp1 == '\0' || *cp1 == ':' || isspace(*cp1)) {
			if (!alloc_to_espace(i)) {
				put_up_arrow(cp - cp_start, _("Equation number out of range."));
				return false;
			}
			cp = cp1;
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
		} else {
			cp--;
		}
	}
#if	SHELL_OUT
/* handle shell escape */
	if (*cp == '!') {
		if (security_level > 0) {
			error(_("Shelling out disabled by security level."));
			return false;
		}
		cp = skip_space(cp + 1);
		if (*cp == '\0' && security_level < 0) {
			error(_("Shelling out disabled by m4."));
			return false;
		}
		cp1 = getenv("SHELL");
		if (cp1 == NULL) {
#if	MINGW
			cp1 = "cmd";
#else
			cp1 = "/bin/sh";
#endif
		}
#if	0
		if (access(cp1, X_OK)) {
			perror(cp1);
			error(_("Shell not found or not executable, check SHELL environment variable."));
			return false;
		}
#endif
		rv = shell_out(*cp ? cp : cp1);
		return !rv;
	}
#endif
#if	HELP
/* a quick way to get help */
	if (*cp == '?') {
		cp = skip_space(cp + 1);
		input_column += (cp - cp_start);
		return(help_cmd(cp));
	}
#endif
/* See if the string pointed to by cp is a command. */
/* If so, execute it. */
do_repeat:
	cp1 = cp;
	while (*cp1 && !isspace(*cp1))
		cp1++;
	len = cp1 - cp;	/* length of possible command name in cp */

#define COMPARE_COMMAND_NAME(str)	(len >= min(CMD_REQUIRED_NCHARS, strlen(str)) \
					&& len <= strlen(str) && strncasecmp(cp, str, len) == 0)

	if (COMPARE_COMMAND_NAME("repeat")) {
		our_repeat_flag = true;
		cp = skip_space(cp1);
		goto do_repeat;
	}
	for (i = 0; i < ARR_CNT(com_list); i++) {
		if (COMPARE_COMMAND_NAME(com_list[i].name)
		    || (com_list[i].secondary_name && COMPARE_COMMAND_NAME(com_list[i].secondary_name))) {
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
				gfp_filename = NULL;
				for (i1 = strlen(buf2) - 1; i1 >= 0; i1--) {
					if (buf2[i1] == '>') {
						gfp_filename = skip_space(&buf2[i1+1]);
						if (i1 && buf2[i1-1] == '>') {
							i1--;
							append_flag = true;
						}
						buf2[i1] = '\0';
						break;
					}
				}
				if (gfp_filename) {
					if (append_flag) {
						fp = fopen(gfp_filename, "a");
					} else {
						fp = fopen(gfp_filename, "w");
					}
					if (fp == NULL) {
						perror(gfp_filename);
						error(_("Can't open redirected output file for writing."));
						gfp_filename = NULL;
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
			show_usage = true;
			repeat_flag = our_repeat_flag;
			rv = (*com_list[i].func)(buf2);
			repeat_flag = false;
#if	!SECURE
			if (fp && gfp != default_out) {
				if (gfp != stdout && gfp != stderr)
					fclose(gfp);
				gfp = default_out;
			}
			gfp_filename = NULL;
#endif
#if	!SILENT
			if (show_usage && !rv && debug_level >= 0) {
				printf("Command usage: %s %s\n", com_list[i].name, com_list[i].usage);
			}
#endif
			return rv;
		}
	}
	if (our_repeat_flag) {
		error(_("Follow \"repeat\" with a command to automatically repeat."));
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
		set_color(3);		/* blue prompt */
		input_column = printf("%d%s", cur_equation + 1, html_flag ? HTML_PROMPT_STR : PROMPT_STR);
		default_color();
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

#if	SHELL_OUT
/*
 * Execute a shell command.  Note that system(3) requires "/bin/sh".
 *
 * Returns exit status of command (0 if no error).
 */
int
shell_out(cp)
char	*cp;	/* shell command string */
{
	int	rv;

	if (security_level > 0) {
		error(_("Shelling out disabled by security level."));
		return -1;
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

#if	HELP

/*
 * Display command usage info in color.
 *
 * Return number of lines displayed.
 */
int
display_usage(pstr, i)
char	*pstr;	/* prefix string */
int	i;
{
	int	len = 0;

	len += fprintf(gfp, "%s", pstr);
	set_color(0);
	len += fprintf(gfp, "%s", com_list[i].name);
	default_color();
	len += fprintf(gfp, " %s\n", com_list[i].usage);
	if (screen_columns && len > screen_columns) {
		return 2;
	} else {
		return 1;
	}
}

/*
 * Output command info and usage.
 *
 * Return the number of lines output.
 */
int
display_command(i)
int	i;	/* command table index of command */
{
	int	rows = 2;

	fprintf(gfp, "%s - %s\n", com_list[i].name, com_list[i].info);
	rows += display_usage("Usage: ", i);
	if (com_list[i].secondary_name) {
		fprintf(gfp, "Alternate name for this command: %s\n", com_list[i].secondary_name);
		rows++;
	}
	if (com_list[i].extra) {
		fprintf(gfp, "%s\n", com_list[i].extra);
		rows++;
	}
	fprintf(gfp, "\n");
#if	DEBUG
	if (com_list[i].secondary_name && com_list[i].extra) {
		error_bug("Alternate name and extra info fields both set for this command, only one or the other is currently allowed.");
	}
#endif
	return rows;
}

/*
 * Output repeat command info and usage.
 *
 * Return the number of lines output.
 */
int
display_repeat_command(void)
{
	EP("repeat - Automatically repeat the following command over and over.");
	fprintf(gfp, "Usage: ");
	set_color(0);
	fprintf(gfp, "repeat");
	default_color();
	fprintf(gfp, " command arguments\n");
	EP("Not all commands are repeatable.\n");
	return 3;
}

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

/*
 * Display a row of dashes to underline a title.
 */
void
underline_title(count)
int	count;
{
#if	!NOT80COLUMNS
	int	i;

	for (i = 1; i < count; i++) {
		fprintf(gfp, "-");
	}
#endif
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
intro:
		/* default help text: */
		SP("Mathomatic is a Computer Algebra System (CAS) and calculator program.");
#if	!LIBRARY
		SP("To see helpful interactive examples, type \"help examples\".");
#endif
		SP("Type \"help equations\" for help with entering expressions and equations.");
		SP("Type \"help all\" for a summary and syntax of all commands.");
		SP("Type \"help usage\" to display the syntax of all commands.");
		SP("Other help topics: constants, color, license, bugs, geometry, conversions.");
		SP("\"help\" or \"?\" followed by a command name will give info on that command.");
		fprintf(gfp, "These are the %d commands for this version of Mathomatic:\n", ARR_CNT(com_list));
		set_color(0);
		for (i = 0; i < ARR_CNT(com_list); i++) {
			if ((i % 5) == 0)
				fprintf(gfp, "\n");
			j = 15 - fprintf(gfp, "%s", com_list[i].name);
			for (; j > 0; j--)
				fprintf(gfp, " ");
		}
		default_color();

		SP("\n\nTo see what is allowed at the main prompt, type \"help main\".");
		EP("For more help, go to the official website: www.mathomatic.org");
	} else if (strcasecmp(cp, "table") == 0) {
		fprintf(gfp, "Mathomatic version %s Quick Reference Card\n", VERSION);
		fprintf(gfp, "Command\tUsage\tNotes\n");
		for (i = 0; i < ARR_CNT(com_list); i++) {
			fprintf(gfp, "%s", com_list[i].name);
			fprintf(gfp, "\t%s %s", com_list[i].name, com_list[i].usage);
			if (com_list[i].secondary_name) {
				fprintf(gfp, "\tAlternate name for this command: %s", com_list[i].secondary_name);
			} else if (com_list[i].extra) {
				fprintf(gfp, "\t%s", com_list[i].extra);
			} else {
				fprintf(gfp, "\t%s", com_list[i].info);
			}
			fprintf(gfp, "\n");
		}
		return true;
	}

next_argument:
	cp1 = cp;
	while (*cp1 && !isspace(*cp1))
		cp1++;
	if (cp1 != cp) {
#if	LIBRARY
		if (html_flag) {
#else
		if (html_flag && gfp == stdout) {
#endif
			EP("<hr>");
		} else {
#if	NOT80COLUMNS
			EP("**********");
#else
			EP("*******************************************************************************");
#endif
		}
		if (gfp != stdout) {
			EP("");
		}
		/* first, see if the argument matches any command names */
		flag = false;
		for (i = 0; i < ARR_CNT(com_list); i++) {
			if (strncasecmp(cp, com_list[i].name, cp1 - cp) == 0
			    || (com_list[i].secondary_name && strncasecmp(cp, com_list[i].secondary_name, cp1 - cp) == 0)) {
				display_command(i);
				flag = true;
			}
		}
		if (strncasecmp(cp, "repeat", cp1 - cp) == 0) {
			display_repeat_command();
			flag = true;
		}
		if (flag) {
			cp = skip_space(cp1);
			goto next_argument;
		}
#if	!LIBRARY
		if (strncasecmp(cp, "startup", cp1 - cp) == 0) {
			display_startup_message(gfp);
			goto next_space;
		}
		if (strncasecmp(cp, "options", cp1 - cp) == 0) {
			usage(gfp);
			goto next_space;
		}
		if (strncasecmp(cp, "examples", cp1 - cp) == 0) {
			return read_examples(example_strings);
		}
#endif
		if (strncasecmp(cp, "geometry", cp1 - cp) == 0) {
			underline_title(EP("Commonly used standard (Euclidean) geometric formulas"));
			return read_examples(geometry_strings);
		}
		if (strncasecmp(cp, "conversions", cp1 - cp) == 0) {
			SP("Commonly used metric/English conversions.");
			SP("Select the equation you want and type the unit name you want, to solve for it.");
			EP("Then type \"repeat calculate\" for units conversion and trying different values.");
			underline_title(EP("These values are correct for the US and UK."));
			return read_examples(conversion_strings);
		}
		if (strncasecmp(cp, "main", cp1 - cp) == 0 || strncasecmp(cp, "prompt", cp1 - cp) == 0) {
			EP("At the Mathomatic main prompt, you may enter:\n");

			EP("  * a numerical expression, which is instantly evaluated and displayed with");
			EP("    the calculate command (autocalc) (see \"help constants\"),");
			EP("  * an algebraic expression or equation, which is stored and made the current");
			EP("    equation (see \"help expressions\"),");
			EP("  * a variable to solve the current equation for (autosolve),");
			EP("  * an equation number to select as the current equation (autoselect),");
			EP("  * a slash (/) or pound sign (#) followed by a variable name to search all");
			EP("    equations spaces forward or backward for,");
			EP("  * a Mathomatic command (see \"help all\"),");
			EP("  * a question mark (?) for quick, short help (same as the help command),");
			EP("  * a semicolon (;) followed by a comment (everything on a line after a");
			EP("    semicolon is ignored),");
#if	SHELL_OUT
			EP("  * or an exclamation point (!) followed by a shell or system command. \"!\" by");
			EP("    itself invokes the default shell.\n");
#else
			EP("  * shelling out (!) is disabled by security level or system type.\n");
#endif

			SP("If a colon (:) starts the line, preceding any of the above input to the main");
			SP("prompt, it will always return with successful status, preventing any current");
			EP("read command operation from aborting due to an error return status.");

			goto next_space;
		}
		if (strncasecmp(cp, "copyrights", cp1 - cp) == 0
		    || strncasecmp(cp, "licenses", cp1 - cp) == 0 || strncasecmp(cp, "licences", cp1 - cp) == 0) {
			underline_title(EP("Copyright and License for Mathomatic"));

			fprintf(gfp, "%s", license_string);
			goto next_space;
		}
		if (strncasecmp(cp, "bugs", cp1 - cp) == 0) {
			SP("Please report bugs on the Mathomatic project site on Launchpad.net:");
			EP("https://launchpad.net/mathomatic");
			SP("Bug tracking is here:");
			EP("https://bugs.launchpad.net/mathomatic\n");

			SP("Launchpad features a complete bug management system.");
			EP("Please include the following information (from the version command):\n");

			version_report();

#if	linux
			SP("\nIn Debian, a convenient way to report bugs from the shell command line");
			EP("is \"reportbug mathomatic\".  In Ubuntu, use \"apport-bug mathomatic\".");
#endif
			goto next_space;
		}
		if (strncasecmp(cp, "usage", cp1 - cp) == 0) {
			underline_title(EP("Mathomatic Command Usage Syntax"));
			for (i = 0, row = 3; i < ARR_CNT(com_list);) {
				row += display_usage("", i);
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
			goto next_space;
		}
		if (cp[0] == '!') {
			SP("A command preceded by an exclamation point (such as \"!ls\") is taken to");
			SP("be a shell command and is passed unchanged to the shell (/bin/sh) when");
			SP("Mathomatic is not in secure mode.  \"!\" by itself invokes the default shell,");
			EP("which is specified in the SHELL environment variable.\n");

			EP("\"!\" is also the factorial operator.");
			goto next_space;
		}
		if (is_mathomatic_operator(cp[0]) || strncasecmp(cp, "operators", cp1 - cp) == 0
		    || strncasecmp(cp, "expressions", cp1 - cp) == 0 || strncasecmp(cp, "equations", cp1 - cp) == 0) {
			SP("To enter an expression or equation, simply type it in at the prompt.");
			EP("Operators have precedence decreasing as indicated:\n");

			EP("    ! factorial (same as gamma(x+1) function; highest precedence)");
			EP("    ** or ^ power (exponentiation; high precedence)");
			EP("    * multiply      / divide        % modulus       // integral divide");
			EP("    + add           - subtract or negate");
			EP("    = equate (lowest precedence)\n");

			SP("Multiple operators of the same precedence level are grouped left to right.");
			SP("Parentheses are used to override operator precedence and group things together.");
			EP("Valid parentheses characters are () and {}.\n");

			SP("Variables consist of any combination of letters, digits, and underscores (_),");
			SP("and \"set special_variable_characters\".  Variables never start with a digit.");
			EP("Predefined variables follow:\n");

			EP("    sign, sign1, sign2, ... - may only be +1 or -1");
			EP("    integer, integer1, ... - may be any integer value\n");

			SP("Absolute value notation \"|x|\" and dual polarity \"+/-x\" are understood.");
			EP("Type \"help constants\" for help with entering Mathomatic constants.");
			goto next_space;
		}
		if (isdigit(cp[0]) || cp[0] == '.'
		    || strncasecmp(cp, "constants", cp1 - cp) == 0 || strncasecmp(cp, "decimal", cp1 - cp) == 0
		    || strncasecmp(cp, "floats", cp1 - cp) == 0 || strncasecmp(cp, "doubles", cp1 - cp) == 0
		    || strncasecmp(cp, "hexadecimal", cp1 - cp) == 0) {
			SP("Constants are displayed as decimal, double precision floating point values");
			SP("with about 14 digits accuracy.  They can be entered in standard, scientific,");
			SP("or hexadecimal notation.  Any constant can be entered in hexadecimal (base 16)");
			SP("by prepending \"0x\" to it.  Excepting named constants, constants always start");
			EP("with a decimal digit (0..9) or a period.\n");

			EP("Named constants follow:\n");

			EP("    e or e# - the universal constant e (2.7182818284...)");
			EP("    pi or pi# - the universal constant pi (3.1415926535...)");
			EP("    i or i# - the imaginary unit (square root of -1)");
			EP("The above constants may also be used anywhere variables are required.");
			EP("    inf - floating point infinity constant");
			EP("    nan - invalid floating point result\n");

			fprintf(gfp, "The largest valid constant is +/-%.7g (slightly less than 2^1024).\n", DBL_MAX);
			fprintf(gfp, "The smallest valid constant is +/-%.7g or 0.\n", DBL_MIN);
			goto next_space;
		}
		if (strncasecmp(cp, "introduction", cp1 - cp) == 0) {
			cp = skip_space(cp1);
			goto intro;
		}
		if (strncasecmp(cp, "colors", cp1 - cp) == 0 || strncasecmp(cp, "colours", cp1 - cp) == 0) {
			if (color_flag) {
				fprintf(gfp, "Color mode is currently on, bold mode is currently %s.\n", bold_colors ? "on" : "off");
				if (html_flag) {
					EP("HTML mode is currently on, so color mode is HTML.");
				} else {
#if	WIN32_CONSOLE_COLORS
					EP("Using WIN32 CONSOLE color mode.");
#else
					EP("Using ANSI color mode.");
#endif
				}
				if (display_all_colors()) {
					fprintf(gfp, " are the available colors.\n");
				} else {
					fprintf(gfp, "\n");
				}
			} else {
				fprintf(gfp, "Color mode is currently turned off.\n");
			}

			EP("\nColor mode is toggled by the -c option on the shell command line, like this:\n");

			EP("    $ mathomatic -c\n");

			SP("ANSI color mode is the default, which outputs ANSI terminal escape sequences");
			SP("to make each level of parentheses a different color, improving readability.");
			SP("If ANSI color mode is on, an ANSI compatible terminal emulator is required.");
			SP("If the colors are hard to see, use the -b option instead, which will always");
			EP("turn on bold color mode, increasing the color brightness.\n");

			EP("These and other startup command-line options are listed with \"help options\".\n");

			SP("The Mathomatic set commands \"set color\" and \"set bold color\" also");
			SP("turn on color and bold color mode for the current session.  \"set no bold\"");
			SP("restores dim colors and \"set no color\" always turns color mode off.");
			EP("When Mathomatic exits, these settings are lost, unless \"set save\" was entered.");
			goto next_space;
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
			goto next_space;
		}
		error(_("Unrecognized help topic or command."));
		return false;
	}
	return true;
next_space:
	if (gfp != stdout) {
		EP("");
	}
	cp = skip_space(cp1);
	goto next_argument;
}
#endif
