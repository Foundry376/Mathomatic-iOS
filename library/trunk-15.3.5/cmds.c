/*
 * Mathomatic commands that don't belong anywhere else.
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

#define	OPT_MIN_SIZE	7	/* Minimum size (in tokens) of repeated expressions to find in optimize command. */

static int	sum_product();
static int	complex_func();
static int	elim_sub();

/* Global variables for the optimize command. */
static int	opt_en[N_EQUATIONS];
static int	last_temp_var = 0;

#if	(UNIX || CYGWIN) && !SECURE && !LIBRARY
/*
 * The plot command.
 *
 * All command functions like this return true if successful, or false for failure.
 */
int
plot_cmd(cp)
char	*cp;	/* the command line argument */
{
	char	cl[MAX_CMD_LEN], *cp1 = NULL;
	int	free_cp1 = false;

	if (security_level) {
		error(_("Command disabled by security level."));
		return false;
	}
	if (n_lhs[cur_equation]) {
		if (n_rhs[cur_equation]) {
			cp1 = list_expression(rhs[cur_equation], n_rhs[cur_equation], 3);
		} else {
			cp1 = list_expression(lhs[cur_equation], n_lhs[cur_equation], 3);
		}
	}
	if (cp1)
		free_cp1 = true;
	else
		cp1 = "";
	if (strchr(cp, 'y') || strchr(cp1, 'y')) {
		printf(_("Performing 3D surface plot...\n"));
		snprintf(cl, sizeof(cl), "echo 'set grid; %s; splot %s %s'|gnuplot -persist", plot_prefix, cp, cp1);
	} else {
		printf(_("Performing 2D plot...\n"));
		snprintf(cl, sizeof(cl), "echo 'set grid; %s; plot %s %s'|gnuplot -persist", plot_prefix, cp, cp1);
	}
	if (free_cp1)
		free(cp1);
	if (shell_out(cl)) {
		error(_("Error trying to run gnuplot."));
		printf(_("Shell command line = \"%s\".\n"), cl);
		return false;
#if	!SILENT
	} else if (debug_level > 0) {
		printf(_("Shell command line = \"%s\".\n"), cl);
#endif
	}
	return true;
}
#endif

/*
 * The version command.
 */
int
version_cmd(cp)
char	*cp;	/* the command line argument */
{
	int		rv;	/* return value */

	if (extra_characters(cp))	/* Make sure nothing else is on the command line. */
		return false;
#if	LIBRARY
	free_result_str();
	result_str = strdup(VERSION);
#endif
	rv = version_report();

	fprintf(gfp, _("\nMathomatic is GNU LGPL version 2.1 licensed software,\n"));
	fprintf(gfp, _("meaning it is free software that comes with no warranty.\n"));
	fprintf(gfp, _("Type \"help license\" for the copyright and license.\n"));

	fprintf(gfp, _("\nDocumentation, reference cards, and the newest version of Mathomatic\n"));
	fprintf(gfp, _("are always available at the Mathomatic website: www.mathomatic.org\n"));
	fprintf(gfp, _("Feedback, questions, and contributions in truth are always welcomed.\n"));
	return rv;
}

/*
 * Return the maximum amount of memory (in bytes) that this program will use.
 */
long
max_memory_usage()
{
	return((long) (N_EQUATIONS + 3L) * n_tokens * sizeof(token_type) * 2L);
}

/*
 * Display version info.
 */
int
version_report()
{
	long	l;

	fprintf(gfp, _("Mathomatic version %s\n"), VERSION);
	fprintf(gfp, _("Compile-time defines used: "));
#if	LINUX
	fprintf(gfp, "LINUX ");
#endif
#if	UNIX
	fprintf(gfp, "UNIX ");
#endif
#if	CYGWIN
	fprintf(gfp, "CYGWIN ");
#endif
#if	HANDHELD
	fprintf(gfp, "HANDHELD ");
#endif
#if	READLINE
	fprintf(gfp, "READLINE ");
#endif
#if	SILENT
	fprintf(gfp, "SILENT ");
#endif
#if	LIBRARY
	fprintf(gfp, "LIBRARY ");
#endif
#if	SECURE
	fprintf(gfp, "SECURE ");
#endif
#if	TIMEOUT_SECONDS
	fprintf(gfp, "TIMEOUT_SECONDS=%d ", TIMEOUT_SECONDS);
#endif
#if	I18N
	fprintf(gfp, "I18N ");
#endif
#if	NO_COLOR
	fprintf(gfp, "NO_COLOR ");
#endif

	fprintf(gfp, "\nsizeof(int) = %u bytes, sizeof(long) = %u bytes.\n\n", (unsigned) sizeof(int), (unsigned) sizeof(long));

	fprintf(gfp, _("The current expression array size is %d tokens,\n"), n_tokens);
	l = max_memory_usage() / 1000L;
	if (l >= 10000L) {
		fprintf(gfp, _("making the maximum memory usage approximately %ld megabytes.\n"), l / 1000L);
	} else {
		fprintf(gfp, _("making the maximum memory usage approximately %ld kilobytes.\n"), l);
	}
#if	SECURE
	fprintf(gfp, _("Compiled for maximum security.\n"));
#else
	fprintf(gfp, _("The current security level is %d.\n"), security_level);
#endif
	return true;
}

/*
 * The solve command.
 *
 * Return 0 on failure, 1 on total success,
 * or 2 if partial success (solved, but solutions didn't verify when doing "solve verify").
 */
int
solve_cmd(cp)
char	*cp;
{
	int		i, j;
	char		buf[MAX_CMD_LEN];
	int		diff_sign;
	int		verify_flag = false, plural_flag = false, once_through, contains_infinity;
	char		*cp1, *cp_start;
	token_type	want;

	cp_start = cp;
	if (cur_equation < 0 || cur_equation >= n_equations || n_lhs[cur_equation] <= 0 || n_rhs[cur_equation] <= 0) {
		error(_("No current equation."));
		return false;
	}
	i = next_espace();
	if (strcmp_tospace(cp, "verify") == 0) {
		verify_flag = true;
		cp = skip_param(cp);
	}
	if (strcmp_tospace(cp, "for") == 0) {
		cp1 = skip_param(cp);
		if (*cp1) {
			cp = cp1;
		}
	}
	if (*cp == '\0') {
		my_strlcpy(prompt_str, _("Enter variable to solve for: "), sizeof(prompt_str));
		if ((cp = get_string(buf, sizeof(buf))) == NULL) {
			return false;
		}
		cp_start = cp;
	}
	input_column += (cp - cp_start);
	if ((cp = parse_equation(i, cp)) == NULL) {
		return false;
	}
	if (verify_flag) {
		if (n_lhs[i] != 1 || n_rhs[i] != 0 || lhs[i][0].kind != VARIABLE) {
			n_lhs[i] = 0;
			n_rhs[i] = 0;
			error(_("Variable required."));
			return false;
		}
		want = lhs[i][0];
		copy_espace(cur_equation, i);
		if (solve_sub(&want, 1, lhs[cur_equation], &n_lhs[cur_equation], rhs[cur_equation], &n_rhs[cur_equation]) > 0) {
			return_result(cur_equation);
			if (!solved_equation(cur_equation) || lhs[cur_equation][0].token.variable != want.token.variable) {
				error(_("Result not a normally solved equation, so cannot verify."));
				n_lhs[i] = 0;
				n_rhs[i] = 0;
				return 1;
			}
			if (compare_es(cur_equation, i)) {
				debug_string(0, _("Nothing was done, so no need to verify solutions."));
				n_lhs[i] = 0;
				n_rhs[i] = 0;
				return 1;
			}
			for (j = 0; j < n_rhs[cur_equation]; j += 2) {
				if (rhs[cur_equation][j].kind == VARIABLE && (rhs[cur_equation][j].token.variable & VAR_MASK) == SIGN) {
					plural_flag = true;
					break;
				}
			}
			subst_var_with_exp(lhs[i], &n_lhs[i], rhs[cur_equation], n_rhs[cur_equation], want.token.variable);
			subst_var_with_exp(rhs[i], &n_rhs[i], rhs[cur_equation], n_rhs[cur_equation], want.token.variable);
			calc_simp(lhs[i], &n_lhs[i]);
			calc_simp(rhs[i], &n_rhs[i]);
			once_through = false;
check_result:
			contains_infinity = (exp_contains_infinity(lhs[i], n_lhs[i])
			    || exp_contains_infinity(rhs[i], n_rhs[i]));
                        if (se_compare(lhs[i], n_lhs[i], rhs[i], n_rhs[i], &diff_sign) && !diff_sign) {
				if (plural_flag)
					printf(_("Solutions verified.\n"));
				else
					printf(_("Solution verified.\n"));
				n_lhs[i] = 0;
				n_rhs[i] = 0;
				if (contains_infinity) {
					error(_("Solution might be incorrect because it contains infinity or NaN."));
					return 2;
				} else {
					return 1;
				}
			} else {
				if (!contains_infinity && !once_through) {
					symb_flag = symblify;
					simpa_repeat_side(lhs[i], &n_lhs[i], true, false);
					simpa_repeat_side(rhs[i], &n_rhs[i], true, false);
					symb_flag = false;
					once_through = true;
					goto check_result;
				}
				if (contains_infinity) {
					error(_("Solution might be incorrect because it contains infinity or NaN."));
				} else {
					if (plural_flag)
						error(_("Solutions might be incorrect."));
					else
						error(_("Solution might be incorrect."));
				}
#if	1	/* set to #if 0 for testing, #if 1 for normal operation */
				n_lhs[i] = 0;
				n_rhs[i] = 0;
#endif
				return 2;
			}
		} else {
#if	!SILENT
			printf(_("Solve failed.\n"));
#endif
		}
	} else {
		if (solve_espace(i, cur_equation)) {
			return return_result(cur_equation);
		}
	}
	n_lhs[i] = 0;
	n_rhs[i] = 0;
	return false;
}

/*
 * The sum command.
 */
int
sum_cmd(cp)
char	*cp;
{
	return sum_product(cp, false);
}

/*
 * The product command.
 */
int
product_cmd(cp)
char	*cp;
{
	return sum_product(cp, true);
}

/*
 * Common function for the sum and product commands.
 */
static int
sum_product(cp, product_flag)
char	*cp;		/* the command line */
int	product_flag;	/* true for product, otherwise sum */
{
	int		i;
	long		v = 0;			/* Mathomatic variable */
	double		start, end, step = 1.0;
	int		result_equation;
	int		n, ns;
	token_type	*dest, *source;
	int		count_down;		/* if true, count down, otherwise count up */
	char		*cp1, buf[MAX_CMD_LEN];

	if (current_not_defined()) {
		return false;
	}
	result_equation = next_espace();
	if (n_rhs[cur_equation]) {
		ns = n_rhs[cur_equation];
		source = rhs[cur_equation];
		dest = rhs[result_equation];
	} else {
		ns = n_lhs[cur_equation];
		source = lhs[cur_equation];
		dest = lhs[result_equation];
	}
	if (*cp) {
		cp = parse_var2(&v, cp);
		if (cp == NULL) {
			return false;
		}
	}
	if (no_vars(source, ns, &v)) {
		error(_("Current expression contains no variables."));
		return false;
	}
	if (v == 0) {
		if (!prompt_var(&v)) {
			return false;
		}
	}
	if (!found_var(source, ns, v)) {
		error(_("Variable not found."));
		return false;
	}
	if (*cp) {
		if (*cp == '=') {
			cp++;
		}
		cp1 = cp;
	} else {
		list_var(v, 0);
		snprintf(prompt_str, sizeof(prompt_str), "%s = ", var_str);
		if ((cp1 = get_string(buf, sizeof(buf))) == NULL)
			return false;
	}
	start = strtod(cp1, &cp);
	if (cp1 == cp) {
		error(_("Number expected."));
		return false;
	}
	if (fabs(start) >= MAX_K_INTEGER) {
		error(_("Number too large."));
		return false;
	}
	cp = skip_space(cp);
	if (strcmp_tospace(cp, "to") == 0) {
		cp = skip_param(cp);
	}
	if (*cp) {
		cp1 = cp;
	} else {
		my_strlcpy(prompt_str, _("To: "), sizeof(prompt_str));
		if ((cp1 = get_string(buf, sizeof(buf))) == NULL)
			return false;
	}
	end = strtod(cp1, &cp);
	if (cp1 == cp) {
		error(_("Number expected."));
		return false;
	}
	if (fabs(end) >= MAX_K_INTEGER) {
		error(_("Number too large."));
		return false;
	}
	cp = skip_space(cp);
	if (*cp) {
		cp1 = cp;
		step = fabs(strtod(cp1, &cp));
		if (cp1 == cp || step <= 0.0 || step >= MAX_K_INTEGER) {
			error(_("Invalid step."));
			return false;
		}
	}
	if (extra_characters(cp))
		return false;
	count_down = (end < start);
	if (fmod(fabs(start - end) / step, 1.0) != 0.0) {
		warning(_("End value not reached."));
	}
	if (product_flag) {
		dest[0] = one_token;
	} else {
		dest[0] = zero_token;
	}
	n = 1;
	for (; count_down ? (start >= end) : (start <= end); count_down ? (start -= step) : (start += step)) {
		if (n + 1 + ns > n_tokens) {
			error_huge();
		}
		blt(tlhs, source, ns * sizeof(token_type));
		n_tlhs = ns;
		for (i = 0; i < n_tlhs; i += 2) {
			if (tlhs[i].kind == VARIABLE && tlhs[i].token.variable == v) {
				tlhs[i].kind = CONSTANT;
				tlhs[i].token.constant = start;
			}
		}
		for (i = 0; i < n_tlhs; i++) {
			tlhs[i].level++;
		}
		for (i = 0; i < n; i++) {
			dest[i].level++;
		}
		dest[n].kind = OPERATOR;
		dest[n].level = 1;
		if (product_flag) {
			dest[n].token.operatr = TIMES;
		} else {
			dest[n].token.operatr = PLUS;
		}
		n++;
		blt(&dest[n], tlhs, n_tlhs * sizeof(token_type));
		n += n_tlhs;
		calc_simp(dest, &n);
		side_debug(1, dest, n);
	}
	if (n_rhs[cur_equation]) {
		n_rhs[result_equation] = n;
		blt(lhs[result_equation], lhs[cur_equation], n_lhs[cur_equation] * sizeof(token_type));
		n_lhs[result_equation] = n_lhs[cur_equation];
	} else {
		n_lhs[result_equation] = n;
	}
	return return_result(result_equation);
}

/*
 * This function is for the "optimize" command.
 * It finds and substitutes all occurrences of the RHS of "en" in "equation".
 * It should be called repeatedly until it returns false.
 */
static int
find_more(equation, np, en)
token_type	*equation;	/* expression to search */
int		*np;		/* pointer to length of expression */
int		en;		/* equation space number */
{
	int	i, j, k;
	int	level;
	int	diff_sign;
	int	found_se;	/* found sub-expression flag */

	if (*np <= 0 || !solved_equation(en)) {
		return false;
	}
	for (level = 1, found_se = true; found_se; level++) {
		for (i = 1, found_se = false; i < *np; i = j + 2) {
			for (j = i; j < *np && equation[j].level >= level; j += 2)
				;
			if (j == i) {
				continue;
			}
			found_se = true;
			k = i - 1;
			if (se_compare(&equation[k], j - k, rhs[en], n_rhs[en], &diff_sign)) {
				if (diff_sign) {
					blt(&equation[i+2], &equation[j], (*np - j) * sizeof(token_type));
					*np -= (j - (i + 2));
					level++;
					equation[k].level = level;
					equation[k].kind = CONSTANT;
					equation[k].token.constant = -1.0;
					k++;
					equation[k].level = level;
					equation[k].kind = OPERATOR;
					equation[k].token.operatr = TIMES;
					k++;
				} else {
					blt(&equation[i], &equation[j], (*np - j) * sizeof(token_type));
					*np -= (j - i);
				}
				equation[k].level = level;
				equation[k].kind = VARIABLE;
				equation[k].token.variable = lhs[en][0].token.variable;
				return true;
			}
		}
	}
	return false;
}

/*
 * This function is for the "optimize" command.
 * It finds and replaces all repeated expressions in "equation" with temporary variables.
 * It also creates a new equation for each temporary variable.
 * It should be called repeatedly until it returns false.
 */
static int
opt_es(equation, np)
token_type	*equation;
int		*np;
{
	int	i, j, k, i1, i2, jj1, k1;
	int	level, level1;
	int	diff_sign;
	int	found_se, found_se1;	/* found sub-expression flags */
	long	v;			/* Mathomatic variable */
	char	var_name_buf[MAX_VAR_LEN];

	if (*np <= 0) {
		return false;
	}
	for (level = 1, found_se = true; found_se; level++) {
		for (i = 1, found_se = false; i < *np; i = j + 2) {
			for (j = i; j < *np && equation[j].level > level; j += 2)
				;
			if (j == i) {
				continue;
			}
			found_se = true;
			k = i - 1;
			if ((j - k) < OPT_MIN_SIZE) {
				continue;
			}
			found_se1 = true;
			for (level1 = 1; found_se1; level1++) {
				for (i1 = 1, found_se1 = false; i1 < *np; i1 = jj1 + 2) {
					for (jj1 = i1; jj1 < *np && equation[jj1].level > level1; jj1 += 2) {
					}
					if (jj1 == i1) {
						continue;
					}
					found_se1 = true;
					if (i1 <= j)
						continue;
					k1 = i1 - 1;
					if ((jj1 - k1) >= OPT_MIN_SIZE
					    && se_compare(&equation[k], j - k, &equation[k1], jj1 - k1, &diff_sign)) {
						snprintf(var_name_buf, sizeof(var_name_buf), "temp%.0d", last_temp_var);
						if (parse_var(&v, var_name_buf) == NULL) {
							return false;	/* can't create "temp" variable */
						}
						last_temp_var++;
						if (last_temp_var < 0) {
							last_temp_var = 0;
						}
						i2 = next_espace();
						lhs[i2][0].level = 1;
						lhs[i2][0].kind = VARIABLE;
						lhs[i2][0].token.variable = v;
						n_lhs[i2] = 1;
						blt(rhs[i2], &equation[k], (j - k) * sizeof(token_type));
						n_rhs[i2] = j - k;
						if (diff_sign) {
							blt(&equation[i1+2], &equation[jj1], (*np - jj1) * sizeof(token_type));
							*np -= (jj1 - (i1 + 2));
							level1++;
							equation[k1].level = level1;
							equation[k1].kind = CONSTANT;
							equation[k1].token.constant = -1.0;
							k1++;
							equation[k1].level = level1;
							equation[k1].kind = OPERATOR;
							equation[k1].token.operatr = TIMES;
							k1++;
						} else {
							blt(&equation[i1], &equation[jj1], (*np - jj1) * sizeof(token_type));
							*np -= (jj1 - i1);
						}
						equation[k1].level = level1;
						equation[k1].kind = VARIABLE;
						equation[k1].token.variable = v;
						blt(&equation[i], &equation[j], (*np - j) * sizeof(token_type));
						*np -= j - i;
						equation[k].level = level;
						equation[k].kind = VARIABLE;
						equation[k].token.variable = v;
						while (find_more(equation, np, i2))
							;
						simp_loop(rhs[i2], &n_rhs[i2]);
						simp_loop(equation, np);
						for (i = 0; opt_en[i] >= 0; i++)
							;
						opt_en[i] = i2;
						opt_en[i+1] = -1;
						return true;
					}
				}
			}
		}
	}
	return false;
}

/*
 * The optimize command.
 */
int
optimize_cmd(cp)
char	*cp;
{
	int	i, j, k, i1;
	int	start, stop;
	int	rv = false, flag;
	int	start_en;

	if (!get_range_eol(&cp, &start, &stop)) {
		return false;
	}
	opt_en[0] = -1;
	start_en = 0;
	for (j = i = start; i <= stop; i++) {
		if (n_lhs[i]) {
			j = i;
			simp_equation(i);
		}
	}
	stop = j;
	do {
		flag = false;
		for (i = start; i <= stop; i++) {
			for (j = start; j <= stop; j++) {
				if (i != j) {
					while (find_more(rhs[i], &n_rhs[i], j)) {
						flag = true;
						rv = true;
					}
				}
			}
		}
	} while (flag);
	for (i = start; i <= stop; i++) {
		if (n_lhs[i] == 0)
			continue;
		do {
			flag = false;
			simp_equation(i);
			for (j = 0; opt_en[j] >= 0; j++) {
				simp_equation(opt_en[j]);
				if (i != opt_en[j]) {
					while (find_more(lhs[i], &n_lhs[i], opt_en[j]))
						flag = true;
					while (find_more(rhs[i], &n_rhs[i], opt_en[j]))
						flag = true;
				}
			}
		} while (flag);
		while (opt_es(lhs[i], &n_lhs[i])) {
			rv = true;
		}
		while (opt_es(rhs[i], &n_rhs[i])) {
			rv = true;
		}
		if (rv) {
			for (i1 = start_en; opt_en[i1] >= 0; i1++) {
				for (j = start_en; opt_en[j] >= 0; j++) {
					for (k = j + 1; opt_en[k] >= 0; k++) {
						while (find_more(rhs[opt_en[k]], &n_rhs[opt_en[k]], opt_en[j]))
							;
						while (find_more(rhs[opt_en[j]], &n_rhs[opt_en[j]], opt_en[k]))
							;
					}
				}
				while (opt_es(rhs[opt_en[i1]], &n_rhs[opt_en[i1]]))
					;
			}
			for (; opt_en[start_en] >= 0; start_en++) {
				list_sub(opt_en[start_en]);
			}
			list_sub(i);
		}
	}
	if (!rv) {
		error(_("Unable to find any repeated expressions."));
	}
	return rv;
}

#if	READLINE
/*
 * The push command.
 */
int
push_cmd(cp)
char	*cp;
{
	int	i, j;

	if (!readline_enabled) {
		error(_("Readline is currently turned off."));
		return false;
	}
	if (!get_range_eol(&cp, &i, &j)) {
		return false;
	}
	for (; i <= j; i++) {
		if (n_lhs[i]) {
			if (!push_en(i)) {
				error(_("Push failed."));
				return false;
			}
		}
	}
	debug_string(0, _("Expression pushed.  Press the UP key to access."));
	return true;
}

/*
 * Push an equation space into the readline history.
 *
 * Return true if successful.
 */
int
push_en(en)
int	en;	/* equation space number to push */
{
	char	*cp;

	if (!readline_enabled)
		return false;
	high_prec = true;
	cp = list_equation(en, false);
	high_prec = false;
	if (cp == NULL)
		return false;
	add_history(cp);
	last_history_string = cp;
	return true;
}
#endif

/*
 * Output the current working directory.
 *
 * Return true if successful.
 */
int
output_current_directory(ofp)
FILE	*ofp;	/* output file pointer */
{
#if	!SECURE
	char	buf[PATH_MAX];

	if (security_level < 3 && ofp && getcwd(buf, sizeof(buf))) {
		fprintf(ofp, "directory = %s\n", buf);
		return true;
	}
#endif
	return false;
}

/*
 * Output the current set options in a format suitable for reading back in.
 */
void
output_options(ofp)
FILE	*ofp;	/* output file pointer */
{
	if (ofp == NULL)
		return;

	fprintf(ofp, "precision = %d digits\n", precision);

	if (!autosolve) {
		fprintf(ofp, "no ");
	}
	fprintf(ofp, "autosolve\n");

	if (!autocalc) {
		fprintf(ofp, "no ");
	}
	fprintf(ofp, "autocalc\n");

	if (!autoselect) {
		fprintf(ofp, "no ");
	}
	fprintf(ofp, "autoselect\n");

#if	!SILENT
	fprintf(ofp, "debug_level = %d\n", debug_level);
#endif

	if (!case_sensitive_flag) {
		fprintf(ofp, "no ");
	}
	fprintf(ofp, "case_sensitive\n");

	if (bold_colors && color_flag) {
		fprintf(ofp, "bold color\n");
	} else {
		if (!color_flag) {
			fprintf(ofp, "no color\n");
		} else {
			fprintf(ofp, "no bold color\n");
		}
	}

	if (!display2d) {
		fprintf(ofp, "no ");
	}
	fprintf(ofp, "display2d\n");

	if (!fractions_display) {
		fprintf(ofp, "no ");
	}
	fprintf(ofp, "fractions_display\n");

	if (quiet_mode) {
		fprintf(ofp, "no ");
	}
	fprintf(ofp, "prompt\n");

	if (!preserve_surds) {
		fprintf(ofp, "no ");
	}
	fprintf(ofp, "preserve_surds\n");

	if (!rationalize_denominators) {
		fprintf(ofp, "no ");
	}
	fprintf(ofp, "rationalize_denominators\n");

	fprintf(ofp, "modulus_mode = %d\n", modulus_mode);

	fprintf(ofp, "finance = %d\n", finance_option);

	if (!factor_int_flag) {
		fprintf(ofp, "no ");
	}
	fprintf(ofp, "factor_integers\n");

	if (!right_associative_power) {
		fprintf(ofp, "no ");
	}
	fprintf(ofp, "right_associative_power\n");

	fprintf(ofp, "plot_prefix = %s\n", plot_prefix);

	fprintf(ofp, "special_variable_characters = %s\n", special_variable_characters);
}

/*
 * Skip over a yes/no indicator and return true if *cpp pointed to a negative word.
 */
int
skip_no(cpp)
char	**cpp;
{
	if (strcmp_tospace(*cpp, "no") == 0
	    || strcmp_tospace(*cpp, "not") == 0
	    || strcmp_tospace(*cpp, "off") == 0
	    || strcmp_tospace(*cpp, "false") == 0) {
		*cpp = skip_param(*cpp);
		return true;
	}
	if (strcmp_tospace(*cpp, "yes") == 0
	    || strcmp_tospace(*cpp, "on") == 0
	    || strcmp_tospace(*cpp, "true") == 0) {
		*cpp = skip_param(*cpp);
	}
	return false;
}

#if	!SECURE && !LIBRARY
/*
 * Save set options in the startup file, displaying a confirmation message.
 *
 * Return true if successful.
 */
int
save_set_options()
{
	FILE	*fp;

	if (rc_file[0] == '\0') {
		error(_("Set options startup file name not set."));
		return false;
	}
	if ((fp = fopen(rc_file, "w")) == NULL) {
		perror(rc_file);
		error(_("Unable to write to set options startup file."));
		return false;
	}
	fprintf(fp, "; Mathomatic set options loaded at startup.\n");
	fprintf(fp, "; This file can be edited or deleted.\n\n");
	output_options(fp);
	if (fclose(fp) == 0) {
		printf(_("All options saved in startup file \"%s\".\n"), rc_file);
	} else {
		perror(rc_file);
		error(_("Error saving set options."));
		return false;
	}
	return true;
}
#endif

/*
 * Handle parsing of options for the set command.
 *
 * Return false if error.
 */
int
set_options(cp)
char	*cp;
{
	int	i;
	int	negate;
	char	*cp1, *option_string, *original_string;

	original_string = cp;
try_next_param:
	cp = skip_space(cp);
	if (*cp == '\0') {
		return true;
	}
#if	!SECURE && !LIBRARY
	if (security_level < 3 && strncasecmp(cp, "dir", 3) == 0) {
		cp = skip_param(cp);
		if (*cp == '\0') {
			cp1 = getenv("HOME");
			if (cp1) {
				cp = cp1;
			}
		}
		if (chdir(cp)) {
			error(_("Error changing directory."));
			return false;
		}
		output_current_directory(stdout);
		return true;
	}
#endif
	negate = skip_no(&cp);
	option_string = cp;
	cp = skip_param(cp);
#if	!SILENT
	if (strncasecmp(option_string, "debug", 5) == 0) {
		if (negate) {
			debug_level = 0;
		} else {
			i = decstrtol(cp, &cp1);
			if (cp == cp1) {
				error(_("Please specify the debug level number."));
				return false;
			}
			cp = cp1;
			debug_level = i;
		}
		goto check_return;
	}
#endif
	if (strncasecmp(option_string, "special", 7) == 0) {
		if (negate) {
			special_variable_characters[0] = '\0';
		} else {
			my_strlcpy(special_variable_characters, cp, sizeof(special_variable_characters));
		}
		return true;
	}
	if (strncasecmp(option_string, "plot_prefix", 4) == 0) {
		if (negate) {
			plot_prefix[0] = '\0';
		} else {
			my_strlcpy(plot_prefix, cp, sizeof(plot_prefix));
		}
		return true;
	}
	if (strncasecmp(option_string, "columns", 6) == 0) {
		if (negate) {
			screen_columns = 0;
		} else {
			if (*cp == '\0') {
				get_screen_size();
				goto check_return;
			}
			i = decstrtol(cp, &cp1);
			if (i < 0 || cp == cp1) {
				error(_("Please specify how wide the screen is; 0 = no column checking."));
				return false;
			}
			cp = cp1;
			screen_columns = i;
		}
		goto check_return;
	}
	if (strncasecmp(option_string, "wide", 4) == 0) {
		if (negate) {
			get_screen_size();
		} else {
			screen_columns = 0;
			screen_rows = 0;
		}
		goto try_next_param;
	}
	if (strncasecmp(option_string, "precision", 4) == 0) {
		i = decstrtol(cp, &cp1);
		if (i < 0 || i > 15 || cp == cp1) {
			error(_("Please specify a display precision between 0 and 15 digits."));
			return false;
		}
		precision = i;
		return true;
	}
	if (strcmp_tospace(option_string, "auto") == 0) {
		autosolve = autocalc = autoselect = !negate;
		goto try_next_param;
	}
	if (strncasecmp(option_string, "autosolve", 9) == 0) {
		autosolve = !negate;
		goto try_next_param;
	}
	if (strncasecmp(option_string, "autocalc", 8) == 0) {
		autocalc = !negate;
		goto try_next_param;
	}
	if (strncasecmp(option_string, "autoselect", 10) == 0) {
		autoselect = !negate;
		goto try_next_param;
	}
	if (strncasecmp(option_string, "case", 4) == 0) {
		case_sensitive_flag = !negate;
		goto try_next_param;
	}
	if (strncasecmp(option_string, "display2d", 7) == 0) {
		display2d = !negate;
		goto try_next_param;
	}
	if (strncasecmp(option_string, "fraction", 8) == 0) {
		fractions_display = !negate;
		goto try_next_param;
	}
	if (strncasecmp(option_string, "prompt", 6) == 0) {
		quiet_mode = negate;
		goto try_next_param;
	}
#if	0
	if (strncasecmp(option_string, "html", 4) == 0) {
		html_flag = !negate;
		goto try_next_param;
	}
#endif
	if (strncasecmp(option_string, "preserve", 8) == 0) {
		preserve_surds = !negate;
		goto try_next_param;
	}
	if (strncasecmp(option_string, "rationalize", 11) == 0) {
		rationalize_denominators = !negate;
		goto try_next_param;
	}
	if (strncasecmp(option_string, "modulus_mode", 3) == 0) {
		if (negate) {
			modulus_mode = 0;
		} else {
			i = decstrtol(cp, &cp1);
			if (cp == cp1 || i < 0 || i > 2) {
				error(_("Please specify the modulus mode number (0, 1, or 2)."));
				printf(_("0 means modulus operator (%%) result has same sign as dividend,\n"));
				printf(_("1 means computed result always has same sign as the divisor,\n"));
                		printf(_("2 means the result is always positive or zero.\n"));
				printf(_("Current value is %d.\n"), modulus_mode);
				return false;
			}
			cp = cp1;
			modulus_mode = i;
		}
		goto check_return;
	}
	if (strncasecmp(option_string, "color", 5) == 0) {
		reset_attr();
		color_flag = !negate;
		goto try_next_param;
	}
	if (strncasecmp(option_string, "bold", 4) == 0) {
		reset_attr();
		bold_colors = !negate;
		goto try_next_param;
	}
	if (strncasecmp(option_string, "finance", 7) == 0) {
		if (negate) {
			finance_option = 0;
		} else {
			i = decstrtol(cp, &cp1);
			if (cp == cp1) {
				if (*cp1 == '\0') {
					i = 2;
				} else {
					error(_("Please specify the number of digits."));
					return false;
				}
			}
			if (i < 0 || i > 14) {
				error(_("Minimum is 0, maximum is 14."));
				return false;
			}
			cp = cp1;
			finance_option = i;
		}
		goto check_return;
	}
	if (strncasecmp(option_string, "factor_integers", 6) == 0) {
		factor_int_flag = !negate;
		goto try_next_param;
	}
	if (strncasecmp(option_string, "right_associative_power", 5) == 0) {
		right_associative_power = !negate;
		goto try_next_param;
	}
#if	!SECURE && !LIBRARY
	if (security_level < 2 && strcmp_tospace(option_string, "save") == 0) {
		if (rc_file[0] == '\0') {
			error(_("Set options startup file name not set."));
			return false;
		}
		if (negate) {
			if (unlink(rc_file) == 0) {
				printf(_("Set options startup file \"%s\" removed.\n"), rc_file);
			}
		} else {
			if (!save_set_options())
				return false;
		}
		goto check_return;
	}
#endif
	printf(_("\nCannot process set string \"%s\".\n"), original_string);
	error(_("Unknown set option."));
	return false;

check_return:
	if (extra_characters(cp))
		return false;
	return true;
}

/*
 * The set command.
 */
int
set_cmd(cp)
char	*cp;
{
	if (*cp == '\0') {
		fprintf(gfp, _("Options are set as follows:\n\n"));

		output_options(gfp);

		fprintf(gfp, "columns = %d\n", screen_columns);

		output_current_directory(gfp);
		return true;
	}
	return set_options(cp);
}

/*
 * The echo command.
 */
int
echo_cmd(cp)
char	*cp;
{
	fprintf(gfp, "%s\n", cp);
	return true;
}

/*
 * The pause command.
 */
int
pause_cmd(cp)
char	*cp;
{
#if	LIBRARY
	return true;
#else
	char	*cp1;
	char	buf[MAX_CMD_LEN];

	if (test_mode || !isatty(0)) {
		return true;
	}
	if (*cp == '\0') {
		cp = _("Please press the Enter key");
	}
	snprintf(prompt_str, sizeof(prompt_str), " ==== %s ==== ", cp);
	if ((cp1 = get_string(buf, sizeof(buf))) == NULL) {
		return false;
	}
	if (strncasecmp(cp1, "quit", 4) == 0) {
		return false;
	}
	if (strncasecmp(cp1, "exit", 4) == 0) {
		return false;
	}
	return true;
#endif
}

/*
 * The copy command.
 */
int
copy_cmd(cp)
char	*cp;
{
	int	i, j, k;
	int	i1;
	char	exists[N_EQUATIONS];

	if (!get_range_eol(&cp, &i, &j)) {
		return false;
	}
	for (i1 = 0; i1 < N_EQUATIONS; i1++) {
		exists[i1] = false;
	}
	for (i1 = i; i1 <= j; i1++) {
		if (n_lhs[i1] > 0) {
			exists[i1] = true;
		}
	}
	for (i1 = i; i1 <= j; i1++) {
		if (exists[i1]) {
			k = next_espace();
			copy_espace(i1, k);
			if (!return_result(k)) {
				return false;
			}
		}
	}
	return true;
}

/*
 * Common function for the imaginary and real commands.
 */
static int
complex_func(cp, imag_flag)
char	*cp;		/* the command line */
int	imag_flag;	/* if true, copy the imaginary part, otherwise copy the real part */
{
	int		i, j, k;
	int		beg;
	int		found_imag, has_imag, has_real;
	token_type	*source, *dest;
	int		n1, *nps, *np;
	long		v = IMAGINARY;			/* separation variable */

	if (current_not_defined()) {
		return false;
	}
	i = cur_equation;
	j = next_espace();
	if (n_rhs[i]) {
		source = rhs[i];
		nps = &n_rhs[i];
		dest = rhs[j];
		np = &n_rhs[j];
	} else {
		source = lhs[i];
		nps = &n_lhs[i];
		dest = lhs[j];
		np = &n_lhs[j];
	}
	if (*cp) {
		cp = parse_var2(&v, cp);
		if (cp == NULL) {
			return false;
		}
		if (extra_characters(cp))
			return false;
	}
	simp_loop(source, nps);
	uf_simp(source, nps);
	factorv(source, nps, v);
	partial_flag = false;
	uf_simp(source, nps);
	partial_flag = true;
	n1 = 1;
	dest[0] = zero_token;
	has_imag = has_real = false;
	for (beg = k = 0; beg < *nps; beg = k, k++) {
		for (found_imag = false; k < *nps; k++) {
			if (source[k].level == 1 && source[k].kind == OPERATOR
			    && (source[k].token.operatr == PLUS || source[k].token.operatr == MINUS)) {
				break;
			}
			if (source[k].kind == VARIABLE && source[k].token.variable == v) {
				found_imag = true;
			}
		}
		if (found_imag)
			has_imag = true;
		else
			has_real = true;
		if (found_imag == imag_flag) {
			if (beg == 0) {
				n1 = 0;
			}
			blt(&dest[n1], &source[beg], (k - beg) * sizeof(token_type));
			n1 += (k - beg);
		}
	}
	if (!has_imag || !has_real) {
		warning(_("Expression was not a mix."));
	}
	do {
		simp_loop(dest, &n1);
	} while (factor_plus(dest, &n1, v, 0.0));
	simp_divide(dest, &n1);
	if (n_rhs[i]) {
		blt(lhs[j], lhs[i], n_lhs[i] * sizeof(token_type));
		n_lhs[j] = n_lhs[i];
	}
	*np = n1;
	cur_equation = j;
	return return_result(cur_equation);
}

/*
 * The real command.
 */
int
real_cmd(cp)
char	*cp;
{
	return complex_func(cp, false);
}

/*
 * The imaginary command.
 */
int
imaginary_cmd(cp)
char	*cp;
{
	return complex_func(cp, true);
}

#if	!LIBRARY
/*
 * The tally command.
 */
int
tally_cmd(cp)
char	*cp;
{
	int	i;
	double	count;
	int	arithmetic_mean = false;

	if (strncasecmp(cp, "average", 4) == 0) {
		arithmetic_mean = true;
		cp = skip_param(cp);
	}
	if (extra_characters(cp)) {
		return false;
	}
	trhs[0] = zero_token;
	n_trhs = 1;
	for (count = 0.0;; count++) {
		fprintf(gfp, _("Running total = "));
		list_proc(trhs, n_trhs, false);
		fprintf(gfp, "\n");
		if (count > 0.0) {
			if (arithmetic_mean) {
				/* calculate and display the average */
				blt(tlhs, trhs, n_trhs * sizeof(token_type));
				n_tlhs = n_trhs;
				if ((n_tlhs + 2) > n_tokens) {
					error_huge();
				}
				for (i = 0; i < n_tlhs; i++) {
					tlhs[i].level++;
				}
				tlhs[n_tlhs].kind = OPERATOR;
				tlhs[n_tlhs].level = 1;
				tlhs[n_tlhs].token.operatr = DIVIDE;
				n_tlhs++;
				tlhs[n_tlhs].kind = CONSTANT;
				tlhs[n_tlhs].level = 1;
				tlhs[n_tlhs].token.constant = count;
				n_tlhs++;
				calc_simp(tlhs, &n_tlhs);
				fprintf(gfp, _("Average = "));
				list_proc(tlhs, n_tlhs, false);
				fprintf(gfp, "\n");
			}
		}
		fprintf(gfp, "\n");
		my_strlcpy(prompt_str, _("Enter value: "), sizeof(prompt_str));
		if (!get_expr(tlhs, &n_tlhs)) {
			break;
		}
		if ((n_trhs + 1 + n_tlhs) > n_tokens) {
			error_huge();
		}
		for (i = 0; i < n_tlhs; i++) {
			tlhs[i].level++;
		}
		for (i = 0; i < n_trhs; i++) {
			trhs[i].level++;
		}
		trhs[n_trhs].kind = OPERATOR;
		trhs[n_trhs].level = 1;
		trhs[n_trhs].token.operatr = PLUS;
		n_trhs++;
		blt(&trhs[n_trhs], tlhs, n_tlhs * sizeof(token_type));
		n_trhs += n_tlhs;
		calc_simp(trhs, &n_trhs);
	}
	fprintf(gfp, _("End.\n"));
	return true;
}
#endif

#if	!LIBRARY
/*
 * The calculate command.
 */
int
calculate_cmd(cp)
char	*cp;
{
	int		i, k1, k;
	long		v, last_v, it_v = 0;	/* Mathomatic variables */
	long		counter, counter_max;
	sign_array_type	sa_mark, sa_value;
	long		l, iterations = 1;
	token_type	*source;
	int		n;
	int		diff_sign;
	char		buf[MAX_CMD_LEN], *cp_start;
	int		factor_flag = false, value_entered;

	cp_start = cp;
	if (current_not_defined()) {
		return false;
	}
	for (;; cp = skip_param(cp)) {
		if (strcmp_tospace(cp, "factor") == 0) {
			factor_flag = true;
			continue;
		}
		if (strcmp_tospace(cp, "repeat") == 0) {
			repeat_flag = true;
			continue;
		}
		break;
	}
	i = cur_equation;
	if (n_rhs[i]) {
		source = rhs[i];
		n = n_rhs[i];
	} else {
		source = lhs[i];
		n = n_lhs[i];
	}
	if (*cp) {
		cp = parse_var2(&it_v, cp);
		if (cp == NULL) {
			return false;
		}
		if (!found_var(source, n, it_v)) {
			error(_("Specified iteration variable not found."));
			return false;
		}
		if (*cp == '\0') {
			my_strlcpy(prompt_str, _("Enter number of iterations: "), sizeof(prompt_str));
			if ((cp = get_string(buf, sizeof(buf))) == NULL)
				return false;
		}
		iterations = decstrtol(cp, &cp);
		if (*cp || iterations < 0) {
			error(_("Positive integer required."));
			return false;
		}
		if (iterations == 0) {
			warning(_("Calculation will be iterated until convergence."));
			iterations = LONG_MAX - 1L;
		}
	}
calc_again:
	value_entered = false;
	n_trhs = n;
	blt(trhs, source, n_trhs * sizeof(token_type));
	last_v = 0;
	for (;;) {
		v = -1;
		for (k1 = 0; k1 < n; k1 += 2) {
			if (source[k1].kind == VARIABLE) {
				if (source[k1].token.variable > last_v
				    && (v == -1 || source[k1].token.variable < v))
					v = source[k1].token.variable;
			}
		}
		if (v == -1)
			break;
		last_v = v;
		if ((v & VAR_MASK) <= SIGN || v == it_v) {
			continue;
		}
		list_var(v, 0);
		snprintf(prompt_str, sizeof(prompt_str), _("Enter %s: "), var_str);
		if (!get_expr(tlhs, &n_tlhs)) {
			continue;
		}
		value_entered = true;
		/* Disguise all variables in the entered expression by making them negative. */
		/* That way they won't be improperly substituted in the future. */
		for (k1 = 0; k1 < n_tlhs; k1 += 2)
			if (tlhs[k1].kind == VARIABLE)
				tlhs[k1].token.variable = -tlhs[k1].token.variable;
		subst_var_with_exp(trhs, &n_trhs, tlhs, n_tlhs, v);
	}
	/* Restore disguised variables: */
	for (k1 = 0; k1 < n_trhs; k1 += 2)
		if (trhs[k1].kind == VARIABLE && trhs[k1].token.variable < 0)
			trhs[k1].token.variable = -trhs[k1].token.variable;
	if (it_v) {
		/* Handle the iteration option, where the simplified result is repeatedly plugged into variable it_v. */
		list_var(it_v, 0);
		snprintf(prompt_str, sizeof(prompt_str), _("Enter initial %s: "), var_str);
		while (!get_expr(tes, &n_tes)) {
			return repeat_flag;
		}
		value_entered = true;
		calc_simp(tes, &n_tes);
		blt(tlhs, trhs, n_trhs * sizeof(token_type));
		n_tlhs = n_trhs;
		for (l = 0; l < iterations; l++) {
			side_debug(1, tes, n_tes);
			blt(trhs, tlhs, n_tlhs * sizeof(token_type));
			n_trhs = n_tlhs;
			subst_var_with_exp(trhs, &n_trhs, tes, n_tes, it_v);
			calc_simp(trhs, &n_trhs);
			if (se_compare(trhs, n_trhs, tes, n_tes, &diff_sign) && !diff_sign) {
				fprintf(gfp, _("Convergence reached after %ld iterations.\n"), l + 1);
				break;
			}
			blt(tes, trhs, n_trhs * sizeof(token_type));
			n_tes = n_trhs;
		}
	}
	calc_simp(trhs, &n_trhs);

	/* Now substitute all sign variables with +1 and -1. */
	CLEAR_ARRAY(sa_mark);
	for (k1 = 0; k1 < n_trhs; k1 += 2) {
		if (trhs[k1].kind == VARIABLE && (trhs[k1].token.variable & VAR_MASK) == SIGN) {
			sa_mark[(trhs[k1].token.variable >> VAR_SHIFT) & SUBSCRIPT_MASK] = true;
		}
	}
	for (k1 = 0, k = 0; k1 < ARR_CNT(sa_mark); k1++) {
		if (sa_mark[k1]) {
			k++;
		}
	}
	counter_max = (1L << k) - 1L;
	if (counter_max) {
		fprintf(gfp, _("There are %ld solutions.\n"), counter_max + 1);
	}
	fprintf(gfp, "\n");
	for (counter = 0; counter <= counter_max; counter++) {
		blt(tlhs, trhs, n_trhs * sizeof(token_type));
		n_tlhs = n_trhs;
		for (k1 = 0, k = 0; k1 < ARR_CNT(sa_mark); k1++) {
			if (sa_mark[k1]) {
				sa_value[k1] = (((1L << k) & counter) != 0);
				k++;
			}
		}
		for (k1 = 0; k1 < n_tlhs; k1 += 2) {
			if (tlhs[k1].kind == VARIABLE && (tlhs[k1].token.variable & VAR_MASK) == SIGN) {
				if (sa_value[(tlhs[k1].token.variable >> VAR_SHIFT) & SUBSCRIPT_MASK]) {
					tlhs[k1].kind = CONSTANT;
					tlhs[k1].token.constant = -1.0;
				} else {
					tlhs[k1].kind = CONSTANT;
					tlhs[k1].token.constant = 1.0;
				}
			}
		}
		for (k1 = 0, k = false; k1 < ARR_CNT(sa_mark); k1++) {
			if (sa_mark[k1]) {
				if (k) {
					fprintf(gfp, ", ");
				} else {
					fprintf(gfp, _("Solution number %ld with "), counter + 1);
				}
				list_var((long) SIGN + (((long) k1) << VAR_SHIFT), 0);
				fprintf(gfp, "%s = ", var_str);
				if (sa_value[k1]) {
					fprintf(gfp, "-1");
				} else {
					fprintf(gfp, "1");
				}
				k = true;
			}
		}
		if (k)
			fprintf(gfp, ":\n");
		calc_simp(tlhs, &n_tlhs);
		if (factor_flag) {
			simp_side(tlhs, &n_tlhs);
		}
		fprintf(gfp, " ");
		if (n_rhs[i]) {
			list_proc(lhs[i], n_lhs[i], false);
			fprintf(gfp, " = ");
		}
		list_factor(tlhs, &n_tlhs, factor_flag);
		if (n_tlhs <= 9 && make_fractions(tlhs, &n_tlhs)) {
			group_proc(tlhs, &n_tlhs);
			fprintf(gfp, " = ");
			list_factor(tlhs, &n_tlhs, factor_flag);
		}
		fprintf(gfp, "\n\n");
	}
	if (value_entered && repeat_flag)
		goto calc_again;
	return true;
}
#endif

/*
 * The clear command.
 */
int
clear_cmd(cp)
char	*cp;
{
	int	i, j;
	char	*cp1;

	do {
		cp1 = cp;
		if (is_all(cp)) {
			clear_all();
			return true;
		} else {
			if (!get_range(&cp, &i, &j)) {
				return false;
			}
			if (*cp && cp == cp1) {
				error(_("Invalid argument."));
				return false;
			}
			for (; i <= j; i++) {
				n_lhs[i] = 0;
				n_rhs[i] = 0;
			}
		}
	} while (*cp);
	return true;
}

/*
 * Compare the Right Hand Sides of two equation spaces.
 */
static int
compare_rhs(i, j, diff_signp)
int	i, j;
int	*diff_signp;
{
	int	rv;

/* First, test the compare function by comparing with self: */
	rv = se_compare(rhs[i], n_rhs[i], rhs[i], n_rhs[i], diff_signp);
	if (!rv || *diff_signp) {
		error(_("Too many terms to compare."));
		return false;
	}
/* Now do the requested compare: */
	sign_cmp_flag = true;
	rv = se_compare(rhs[i], n_rhs[i], rhs[j], n_rhs[j], diff_signp);
	sign_cmp_flag = false;
	return rv;
}

/*
 * Compare two equation spaces.
 *
 * Return true if identical.
 */
int
compare_es(i, j)
int	i, j;	/* equation space numbers */
{
	int	rv;
	int	diff_sign_lhs, diff_sign_rhs;

	if (n_lhs[i] == 0 || n_lhs[j] == 0)
		return false;	/* empty equation space */
	if ((n_rhs[i] == 0) != (n_rhs[j] == 0))
		return false;	/* mix of expression and equation */
/* Compare the two left hand sides: */
	sign_cmp_flag = true;
	rv = se_compare(lhs[i], n_lhs[i], lhs[j], n_lhs[j], &diff_sign_lhs);
	sign_cmp_flag = false;
	if (!rv)
		return false;
	if (n_rhs[i] == 0 && n_rhs[j] == 0)
		return !diff_sign_lhs;	/* two expressions, not equations */
/* They are equations, so compare the two right hand sides: */
	sign_cmp_flag = true;
	rv = se_compare(rhs[i], n_rhs[i], rhs[j], n_rhs[j], &diff_sign_rhs);
	sign_cmp_flag = false;
	if (!rv)
		return false;
	return(diff_sign_lhs == diff_sign_rhs);
}

/*
 * The compare command.
 */
int
compare_cmd(cp)
char	*cp;
{
	int		i, j;
	int		diff_sign;
	int		symb = false;

	if (strncasecmp(cp, "symbolic", 3) == 0) {
		symb = true;
		cp = skip_param(cp);
	}
	if (strcmp_tospace(cp, "with") == 0) {
		cp = skip_param(cp);
	}
	i = decstrtol(cp, &cp) - 1;
	if (not_defined(i)) {
		return false;
	}
	if (strcmp_tospace(cp, "with") == 0) {
		cp = skip_param(cp);
	}
	if ((j = get_default_en(cp)) < 0) {
		return false;
	}
	if (i == j) {
		error(_("Cannot compare an equation with itself."));
		return false;
	}
	fprintf(gfp, _("Comparing #%d with #%d...\n"), i + 1, j + 1);
	if (n_rhs[i] == 0 || n_rhs[j] == 0) {
		if (n_rhs[i] == 0 && n_rhs[j] == 0) {
			simp_loop(lhs[i], &n_lhs[i]);
			simp_loop(lhs[j], &n_lhs[j]);
			if (compare_es(i, j)) {
				fprintf(gfp, _("Expressions are identical.\n"));
				return true;
			}
			debug_string(0, _("Completely simplifying both expressions..."));
			symb_flag = symb;
			simpa_repeat_side(lhs[i], &n_lhs[i], false, true);
			simpa_repeat_side(lhs[j], &n_lhs[j], false, true);
			symb_flag = false;
#if	!SILENT
			if (debug_level >= 0) {
				list_sub(i);
				list_sub(j);
			}
#endif
			if (compare_es(i, j)) {
				fprintf(gfp, _("Expressions are identical.\n"));
				return true;
			}
			uf_simp(lhs[i], &n_lhs[i]);
			uf_simp(lhs[j], &n_lhs[j]);
			if (compare_es(i, j)) {
				fprintf(gfp, _("Expressions are identical.\n"));
				return true;
			}
			fprintf(gfp, _("Expressions differ.\n"));
			return false;
		}
		error(_("Cannot compare an equation with a non-equation."));
		return false;
	}
	if (compare_es(i, j)) {
		fprintf(gfp, _("Equations are identical.\n"));
		return true;
	}
	if (solved_equation(i) && solved_equation(j)) {
		simp_loop(rhs[i], &n_rhs[i]);
		simp_loop(rhs[j], &n_rhs[j]);
		if (compare_rhs(i, j, &diff_sign)) {
			goto times_neg1;
		}
		debug_string(0, _("Completely simplifying both equations..."));
		symb_flag = symb;
		simpa_repeat_side(rhs[i], &n_rhs[i], false, true);
		simpa_repeat_side(rhs[j], &n_rhs[j], false, true);
		symb_flag = false;
#if	!SILENT
		if (debug_level >= 0) {
			list_sub(i);
			list_sub(j);
		}
#endif
		if (compare_rhs(i, j, &diff_sign)) {
			goto times_neg1;
		}
		uf_simp(rhs[i], &n_rhs[i]);
		uf_simp(rhs[j], &n_rhs[j]);
		if (compare_rhs(i, j, &diff_sign)) {
			goto times_neg1;
		}
	}
	debug_string(0, _("Solving both equations for zero and unfactoring..."));
	if (solve_sub(&zero_token, 1, lhs[i], &n_lhs[i], rhs[i], &n_rhs[i]) <= 0)
		return false;
	if (solve_sub(&zero_token, 1, lhs[j], &n_lhs[j], rhs[j], &n_rhs[j]) <= 0)
		return false;
	uf_simp(rhs[i], &n_rhs[i]);
	uf_simp(rhs[j], &n_rhs[j]);
	if (compare_rhs(i, j, &diff_sign)) {
		fprintf(gfp, _("Equations are identical.\n"));
		return true;
	}
	debug_string(0, _("Completely simplifying both equations..."));
	symb_flag = symb;
	simpa_repeat_side(rhs[i], &n_rhs[i], false, false);
	simpa_repeat_side(rhs[j], &n_rhs[j], false, false);
	symb_flag = false;
	if (compare_rhs(i, j, &diff_sign)) {
		fprintf(gfp, _("Equations are identical.\n"));
		return true;
	}
	if (solve_sub(&zero_token, 1, lhs[i], &n_lhs[i], rhs[i], &n_rhs[i]) <= 0)
		return false;
	if (solve_sub(&zero_token, 1, lhs[j], &n_lhs[j], rhs[j], &n_rhs[j]) <= 0)
		return false;
	uf_simp(rhs[i], &n_rhs[i]);
	uf_simp(rhs[j], &n_rhs[j]);
	if (compare_rhs(i, j, &diff_sign)) {
		fprintf(gfp, _("Equations are identical.\n"));
		return true;
	}
	fprintf(gfp, _("Equations differ.\n"));
	return false;
times_neg1:
	if (!diff_sign && lhs[i][0].token.variable == lhs[j][0].token.variable) {
		fprintf(gfp, _("Equations are identical.\n"));
		return true;
	}
	fprintf(gfp, _("Variable ("));
	list_proc(lhs[i], n_lhs[i], false);
	fprintf(gfp, _(") in the first equation is equal to ("));
	if (diff_sign) {
		fprintf(gfp, "-");
	}
	list_proc(lhs[j], n_lhs[j], false);
	fprintf(gfp, _(") in the second equation.\n"));
#if	LIBRARY
	return false;
#else
	return 2;
#endif
}

/*
 * Display the specified floating point value.
 * If it is equal to a simple fraction, display that too.
 *
 * Return true if a fraction was displayed.
 */
int
display_fraction(value)
double	value;
{
	double	d4, d5;
	int	rv = false;

	f_to_fraction(value, &d4, &d5);
	fprintf(gfp, "%.*g", precision, value);
	if (d5 != 1.0) {
		fprintf(gfp, " = %.*g/%.*g", precision, d4, precision, d5);
		rv = true;
	}
	fprintf(gfp, "\n");
	return rv;
}

/*
 * The divide command.
 */
int
divide_cmd(cp)
char	*cp;
{
	long		v = 0, v_tmp;		/* Mathomatic variables */
	int		i, j;
	int		nleft = 0, nright = 0;
	double		lcm, d1, d2, d3, d4, d5;
	complexs	c1, c2, c3;

	pull_number = -1;	/* Operands are last two entered expressions when using library. */
	if (*cp) {
		cp = parse_var2(&v, cp);
		if (cp == NULL) {
			return false;
		}
		if (extra_characters(cp))
			return false;
	}
	i = next_espace();
do_repeat:
/* prompt for the two operands */
	my_strlcpy(prompt_str, _("Enter dividend: "), sizeof(prompt_str));
	if (!get_expr(rhs[i], &nright)) {
		return repeat_flag;
	}
	my_strlcpy(prompt_str, _("Enter divisor: "), sizeof(prompt_str));
	if (!get_expr(lhs[i], &nleft)) {
		return repeat_flag;
	}
	fprintf(gfp, "\n");
/* simplify the operand expressions */
	calc_simp(rhs[i], &nright);
	calc_simp(lhs[i], &nleft);
/* if division by zero, display a warning */
	if (get_constant(lhs[i], nleft, &d2)) {
		check_divide_by_zero(d2);
	}
/* Do constant division if 2 normal numbers were entered */
	if (get_constant(rhs[i], nright, &d1) && get_constant(lhs[i], nleft, &d2)) {
		d3 = gcd_verified(d1, d2);
		d4 = modf(d1 / d2, &d5);
		fprintf(gfp, "%.*g/%.*g = %.*g", precision, d1, precision, d2, precision, d1 / d2);
		if (d3 != 0.0 && (d2 / d3) != 1.0) {
			fprintf(gfp, " = %.*g/%.*g", precision, d1 / d3, precision, d2 / d3);
		}
		fprintf(gfp, _("\nQuotient: %.*g, Remainder: %.*g\n"), precision, d5, precision, d4 * d2);
		d1 = fabs(d1);
		d2 = fabs(d2);
		if (d3 == 0.0) {
			fprintf(gfp, _("No GCD found.\n"));
			if (repeat_flag)
				goto do_repeat;
			return true;
		}
		fprintf(gfp, "GCD = ");
		if (d3 >= 4.0 && factor_one(d3) && !is_prime()) {
			display_unique();
		} else {
			display_fraction(d3);
		}
		lcm = (d1 * d2) / d3;
		fprintf(gfp, "LCM = ");
		if (lcm >= 4.0 && factor_one(lcm) && !is_prime()) {
			display_unique();
		} else {
			display_fraction(lcm);
		}
		if (repeat_flag)
			goto do_repeat;
		return true;
	}
/* else do complex number division if 2 complex numbers were entered */
	if (parse_complex(rhs[i], nright, &c1) && parse_complex(lhs[i], nleft, &c2)) {
		c3 = complex_div(c1, c2);
		fprintf(gfp, _("Result of complex number division:\n"));
		fprintf(gfp, "%.*g %+.*g*i\n\n", precision, c3.re, precision, c3.im);
		if (repeat_flag)
			goto do_repeat;
		return true;
	}
/* else do polynomial division and univariate GCD display */
	v_tmp = v;
	if (poly_div(rhs[i], nright, lhs[i], nleft, &v_tmp)) {
		simp_divide(tlhs, &n_tlhs);
		simp_divide(trhs, &n_trhs);
		list_var(v_tmp, 0);
		fprintf(gfp, _("Polynomial division successful using base variable (%s).\n"), var_str);
		fprintf(gfp, _("The quotient is:\n"));
		fractions_and_group(tlhs, &n_tlhs);
		list_factor(tlhs, &n_tlhs, false);
		fprintf(gfp, _("\n\nThe remainder is:\n"));
		fractions_and_group(trhs, &n_trhs);
		list_factor(trhs, &n_trhs, false);
		fprintf(gfp, "\n");
	} else {
		fprintf(gfp, _("Polynomial division failed.\n"));
	}
	fprintf(gfp, "\n");
	j = poly_gcd(rhs[i], nright, lhs[i], nleft, v);
	if (!j) {
		j = poly_gcd(lhs[i], nleft, rhs[i], nright, v);
	}
	if (j) {
		simp_divide(trhs, &n_trhs);
		fprintf(gfp, _("Polynomial GCD (%d Euclidean algorithm iterations):\n"), j);
		fractions_and_group(trhs, &n_trhs);
		list_factor(trhs, &n_trhs, false);
		fprintf(gfp, "\n");
	} else {
		fprintf(gfp, _("No univariate polynomial GCD found.\n"));
	}
	if (repeat_flag)
		goto do_repeat;
	return true;
}

/*
 * The eliminate command.
 */
int
eliminate_cmd(cp)
char	*cp;
{
	long	v, last_v, v1, va[MAX_VARS];		/* Mathomatic variables */
	int	vc = 0;					/* variable count */
	int	i = 0, n;
	int	success_flag = false, did_something = false, using_flag;
	char	used[N_EQUATIONS];
	char	*cp_start;
	char	buf[MAX_CMD_LEN];

	CLEAR_ARRAY(used);
	if (current_not_defined()) {
		return false;
	}
	if (strcmp_tospace(cp, "repeat") == 0) {
		repeat_flag = true;
		cp = skip_param(cp);
	}
	if (*cp == '\0') {
		my_strlcpy(prompt_str, _("Enter variables to eliminate: "), sizeof(prompt_str));
		cp = get_string(buf, sizeof(buf));
		if (cp == NULL || *cp == '\0') {
			return false;
		}
	}
	cp_start = cp;
next_var:
	if (vc) {
		v = va[--vc];
	} else if (*cp) {
		if (is_all(cp)) {
			cp = skip_param(cp);
			vc = 0;
			last_v = 0;
			for (;;) {
				v1 = -1;
				for (i = 0; i < n_lhs[cur_equation]; i += 2) {
					if (lhs[cur_equation][i].kind == VARIABLE
					    && lhs[cur_equation][i].token.variable > last_v) {
						if (v1 == -1 || lhs[cur_equation][i].token.variable < v1) {
							v1 = lhs[cur_equation][i].token.variable;
						}
					}
				}
				for (i = 0; i < n_rhs[cur_equation]; i += 2) {
					if (rhs[cur_equation][i].kind == VARIABLE
					    && rhs[cur_equation][i].token.variable > last_v) {
						if (v1 == -1 || rhs[cur_equation][i].token.variable < v1) {
							v1 = rhs[cur_equation][i].token.variable;
						}
					}
				}
				if (v1 == -1)
					break;
				last_v = v1;
				if ((v1 & VAR_MASK) > SIGN) {
					if (vc >= ARR_CNT(va)) {
						break;
					}
					va[vc++] = v1;
				}
			}
			goto next_var;
		}
		cp = parse_var2(&v, cp);
		if (cp == NULL) {
			return false;
		}
	} else {
		if (repeat_flag) {
			if (success_flag) {
				success_flag = false;
				cp = cp_start;
				goto next_var;	/* repeat until failure to substitute anything */
			}
		}
		if (did_something) {
			did_something = return_result(cur_equation);
		} else {
			error(_("No substitutions made."));
		}
		return did_something;
	}
	using_flag = (strcmp_tospace(cp, "using") == 0);
	if (using_flag) {
		cp = skip_param(cp);
		i = decstrtol(cp, &cp) - 1;
		if (not_defined(i)) {
			return false;
		}
	}
	if (!var_in_equation(cur_equation, v)) {
#if	!SILENT
		if (!repeat_flag) {
			list_var(v, 0);
			printf(_("Variable (%s) not found in current equation.\n"), var_str);
		}
#endif
		goto next_var;
	}
	if (using_flag) {
		if (!elim_sub(i, v))
			goto next_var;
	} else {
		n = 1;
		i = cur_equation;
		for (;; n++) {
			if (n >= n_equations) {
				goto next_var;
			}
			if (i <= 0)
				i = n_equations - 1;
			else
				i--;
			if (used[i])
				continue;
			if (n_lhs[i] && n_rhs[i] && var_in_equation(i, v)) {
				if (elim_sub(i, v))
					break;
			}
		}
	}
	success_flag = true;
	did_something = true;
	used[i] = true;
	goto next_var;
}

/*
 * Solve equation number i for v and substitute the RHS
 * into all occurrences of v in the current equation, simplifying a little.
 */
static int
elim_sub(i, v)
int	i;	/* equation number */
long	v;	/* Mathomatic variable */
{
	token_type	want;
	int		solved;

	if (i == cur_equation) {
		error(_("Error: source and destination are the same."));
		return false;
	}
	solved = (solved_equation(i) && lhs[i][0].token.variable == v);
#if	!SILENT
	list_var(v, 0);
	if (solved) {
		/* already solved */
		printf(_("Substituting the RHS of equation #%d into the current equation for variable (%s)...\n"), i + 1, var_str);
	} else {
		printf(_("Solving equation #%d for (%s) and substituting into the current equation...\n"), i + 1, var_str);
	}
#endif
	if (!solved) {
		want.level = 1;
		want.kind = VARIABLE;
		want.token.variable = v;
		if (solve_sub(&want, 1, lhs[i], &n_lhs[i], rhs[i], &n_rhs[i]) <= 0) {
			error(_("Solve failed."));
			return false;
		}
	}
	subst_var_with_exp(rhs[cur_equation], &n_rhs[cur_equation], rhs[i], n_rhs[i], v);
	subst_var_with_exp(lhs[cur_equation], &n_lhs[cur_equation], rhs[i], n_rhs[i], v);
	simp_equation(cur_equation);
	return true;
}

/*
 * The display command.
 *
 * Displays equations in multi-line fraction format.
 */
int
display_cmd(cp)
char	*cp;
{
	int	i, j;
	char	*cp1;
	jmp_buf	save_save;
	int	factor_flag, displayed = false;

	factor_flag = (strcmp_tospace(cp, "factor") == 0);
	if (factor_flag) {
		cp = skip_param(cp);
	}
	do {
		cp1 = cp;
		if (!get_range(&cp, &i, &j)) {
			return false;
		}
		if (*cp && cp == cp1) {
			error(_("Invalid argument."));
			return false;
		}
		for (; i <= j; i++) {
			if (n_lhs[i] > 0) {
				blt(save_save, jmp_save, sizeof(jmp_save));
				if (setjmp(jmp_save) != 0) {	/* trap errors */
					blt(jmp_save, save_save, sizeof(jmp_save));
					printf("Skipping equation number %d.\n", i + 1);
					continue;
				}
				make_fractions_and_group(i);
				if (factor_flag || factor_int_flag) {
					factor_int_sub(i);
				}
				blt(jmp_save, save_save, sizeof(jmp_save));
#if     LIBRARY
				free_result_str();
				result_str = list_equation(i, false);
				result_en = i;
#endif
				if (flist_equation(i) > 0) {
					displayed = true;
				}
			}
		}
	} while (*cp);
#if	LIBRARY
	return(result_str != NULL);
#else
	return(displayed);
#endif
}

/*
 * The list command.
 */
int
list_cmd(cp)
char	*cp;
{
	int	i, j;
	char	*cp1;
	int	export_flag = 0;

	if (strncasecmp(cp, "gnuplot", 3) == 0) {
		export_flag = 3;
		cp = skip_param(cp);
	} else if (strncasecmp(cp, "export", 3) == 0) {
		export_flag = 2;
		cp = skip_param(cp);
	} else if (strncasecmp(cp, "maxima", 3) == 0) {
		export_flag = 1;
		cp = skip_param(cp);
	} else if (strncasecmp(cp, "hexadecimal", 3) == 0) {
		export_flag = 4;
		cp = skip_param(cp);
	}
	do {
		cp1 = cp;
		if (!get_range(&cp, &i, &j)) {
			return false;
		}
		if (*cp && cp == cp1) {
			error(_("Invalid argument."));
			return false;
		}
		for (; i <= j; i++) {
			if (n_lhs[i] > 0) {
#if	LIBRARY
				free_result_str();
				result_str = list_equation(i, export_flag);
				result_en = i;
				if (result_str == NULL)
					return false;
				if (gfp == stdout) {
					continue;
				}
#endif
				list1_sub(i, export_flag);
			}
		}
	} while (*cp);
	return true;
}

/*
 * The code command.
 */
int
code_cmd(cp)
char	*cp;
{
	int			i, j;
	enum language_list	language = C;
	int			int_flag = false, displayed = false;
	char			*cp1;

	for (;; cp = skip_param(cp)) {
		if (strcmp_tospace(cp, "c") == 0 || strcmp_tospace(cp, "c++") == 0) {
			language = C;
			continue;
		}
		if (strcmp_tospace(cp, "java") == 0) {
			language = JAVA;
			continue;
		}
		if (strcmp_tospace(cp, "python") == 0) {
			language = PYTHON;
			continue;
		}
		if (strcmp_tospace(cp, "int") == 0 || strcmp_tospace(cp, "integer") == 0) {
			int_flag = true;
			continue;
		}
		break;
	}
	do {
		cp1 = cp;
		if (!get_range(&cp, &i, &j)) {
			return false;
		}
		if (*cp && cp == cp1) {
			error(_("Invalid argument."));
			return false;
		}
		for (; i <= j; i++) {
			if (n_lhs[i] > 0) {
				if (n_rhs[i] == 0 || n_lhs[i] != 1 || lhs[i][0].kind != VARIABLE) {
					warning(_("Can't make assignment statement because this is not an equation."));
				}
				approximate_roots = true;
				simp_i(lhs[i], &n_lhs[i]);
				if (int_flag) {
					uf_repeat_always(lhs[i], &n_lhs[i]);
				}
				if (n_rhs[i] > 0) {
					simp_i(rhs[i], &n_rhs[i]);
					if (int_flag) {
						uf_repeat_always(rhs[i], &n_rhs[i]);
					}
				}
				approximate_roots = false;
				make_fractions_and_group(i);
				if (int_flag && (!int_expr(lhs[i], n_lhs[i]) || !int_expr(rhs[i], n_rhs[i]))) {
					warning(_("Not an integer expression."));
				}
				if (list_c_equation(i, language, int_flag) > 0) {
					displayed = true;
				}
			}
		}
	} while (*cp);
	return displayed;
}

/*
 * Compare function for qsort(3).
 */
static int
vcmp(p1, p2)
sort_type	*p1, *p2;
{
	return(p2->count - p1->count);
}

/*
 * The variables command.
 */
int
variables_cmd(cp)
char	*cp;
{
	int			i, j, k;
	int			i1;
	long			v1, last_v;		/* Mathomatic variables */
	int			vc, cnt;		/* variable counts */
	sort_type		va[MAX_VARS];		/* variable array */
	token_type		*p1;
	int			n1;
	enum language_list	lang_code = 0;		/* default to no programming language */
	int			int_flag = false, imag_flag = false;

	if (strcmp_tospace(cp, "c") == 0 || strcmp_tospace(cp, "c++") == 0) {
		cp = skip_param(cp);
		lang_code = C;
	} else if (strcmp_tospace(cp, "java") == 0) {
		cp = skip_param(cp);
		lang_code = JAVA;
	} else if (strcmp_tospace(cp, "int") == 0 || strcmp_tospace(cp, "integer") == 0) {
		cp = skip_param(cp);
		lang_code = C;
		int_flag = true;
	}
	if (!get_range_eol(&cp, &i, &j)) {
		return false;
	}
	for (k = i; k <= j; k++) {
		if (n_lhs[k] <= 0)
			continue;
		if (n_rhs[k] > 0) {
			p1 = rhs[k];
			n1 = n_rhs[k];
		} else {
			p1 = lhs[k];
			n1 = n_lhs[k];
		}
		for (i1 = 0; i1 < n1; i1 += 2) {
			if (p1[i1].kind == VARIABLE && p1[i1].token.variable == IMAGINARY) {
				imag_flag = true;
				break;
			}
		}
	}
	last_v = 0;
	for (vc = 0;;) {
		if (vc >= ARR_CNT(va)) {
			error(_("Too many variables to list."));
			return false;
		}
		cnt = 0;
		v1 = -1;
		for (k = i; k <= j; k++) {
			if (n_lhs[k] <= 0)
				continue;
			p1 = lhs[k];
			n1 = n_lhs[k];
			for (i1 = 0; i1 < n1; i1 += 2) {
				if (p1[i1].kind == VARIABLE && p1[i1].token.variable > last_v) {
					if (v1 == -1 || p1[i1].token.variable < v1) {
						v1 = p1[i1].token.variable;
						cnt = 1;
					} else if (p1[i1].token.variable == v1) {
						cnt++;
					}
				}
			}
			p1 = rhs[k];
			n1 = n_rhs[k];
			for (i1 = 0; i1 < n1; i1 += 2) {
				if (p1[i1].kind == VARIABLE && p1[i1].token.variable > last_v) {
					if (v1 == -1 || p1[i1].token.variable < v1) {
						v1 = p1[i1].token.variable;
						cnt = 1;
					} else if (p1[i1].token.variable == v1) {
						cnt++;
					}
				}
			}
		}
		if (v1 == -1)
			break;
		last_v = v1;
		va[vc].v = v1;
		va[vc].count = cnt;
		vc++;
	}
	if (vc <= 0)
		return true;
	qsort((char *) va, vc, sizeof(*va), vcmp);
	for (i1 = 0; i1 < vc; i1++) {
		if (lang_code && va[i1].v < SIGN) {
			continue;
		}
		list_var(va[i1].v, lang_code);
		if (lang_code) {
			if (imag_flag) {
				fprintf(gfp, "_Complex ");
			}
			if (int_flag) {
				fprintf(gfp, "int %s;\n", var_str);
			} else {
				fprintf(gfp, "double %s;\n", var_str);
			}
		} else {
			fprintf(gfp, "%s\n", var_str);
		}
	}
	return true;
}

/*
 * The approximate command.
 */
int
approximate_cmd(cp)
char	*cp;
{
	int	i, j;
	char	*cp1;

	do {
		cp1 = cp;
		if (!get_range(&cp, &i, &j)) {
			return false;
		}
		if (*cp && cp == cp1) {
			error(_("Invalid argument."));
			return false;
		}
		for (; i <= j; i++) {
			if (n_lhs[i]) {
				subst_constants(lhs[i], &n_lhs[i]);
				subst_constants(rhs[i], &n_rhs[i]);
				approximate_roots = true;
				simp_equation(i);
				factorv(lhs[i], &n_lhs[i], IMAGINARY);
				if (n_rhs[i]) {
					factorv(rhs[i], &n_rhs[i], IMAGINARY);
				}
				approximate_roots = false;
				if (!return_result(i)) {
					return false;
				}
			}
		}
	} while (*cp);
	return true;
}

/*
 * The replace command.
 */
int
replace_cmd(cp)
char	*cp;
{
	int	i, j;
	int	n;
	long	last_v, v, va[MAX_VARS];	/* Mathomatic variables */
	int	vc;				/* variable count */
	char	*cp_start, *cp1;
	int	found;

	cp_start = cp;
	if (current_not_defined()) {
		return false;
	}
	i = cur_equation;
	for (vc = 0; *cp; vc++) {
		if (strcmp_tospace(cp, "with") == 0) {
			if (vc)
				break;
#if	0	/* allow "with" variable */
			error(_("No variables specified."));
			return false;
#endif
		}
		if (vc >= ARR_CNT(va)) {
			error(_("Too many variables specified."));
			return false;
		}
		cp = parse_var2(&va[vc], cp);
		if (cp == NULL) {
			return false;
		}
		if (!var_in_equation(i, va[vc])) {
			error(_("Variable not found."));
			return false;
		}
	}
	n_tlhs = n_lhs[i];
	blt(tlhs, lhs[i], n_tlhs * sizeof(token_type));
	n_trhs = n_rhs[i];
	blt(trhs, rhs[i], n_trhs * sizeof(token_type));
	last_v = 0;
	for (;;) {
		v = -1;
		for (j = 0; j < n_lhs[i]; j += 2) {
			if (lhs[i][j].kind == VARIABLE) {
				if (lhs[i][j].token.variable > last_v
				    && (v == -1 || lhs[i][j].token.variable < v))
					v = lhs[i][j].token.variable;
			}
		}
		for (j = 0; j < n_rhs[i]; j += 2) {
			if (rhs[i][j].kind == VARIABLE) {
				if (rhs[i][j].token.variable > last_v
				    && (v == -1 || rhs[i][j].token.variable < v))
					v = rhs[i][j].token.variable;
			}
		}
		if (v == -1) {
			break;
		}
		last_v = v;
		if (vc) {
			found = false;
			for (j = 0; j < vc; j++) {
				if (v == va[j])
					found = true;
			}
			if (!found)
				continue;
			if (*cp) {
				if (strcmp_tospace(cp, "with") != 0) {
					return false;
				}
				cp1 = skip_param(cp);
				input_column += (cp1 - cp_start);
				if ((cp1 = parse_expr(scratch, &n, cp1)) == NULL || n <= 0) {
					return false;
				}
				goto do_this;
			}
		}
		list_var(v, 0);
		snprintf(prompt_str, sizeof(prompt_str), _("Enter %s: "), var_str);
		if (!get_expr(scratch, &n)) {
			continue;
		}
do_this:
		/* Disguise all variables in the entered expression by making them negative; */
		/* That way they won't be improperly substituted later, allowing variable interchange. */
		for (j = 0; j < n; j += 2) {
			if (scratch[j].kind == VARIABLE) {
				scratch[j].token.variable = -scratch[j].token.variable;
			}
		}
		subst_var_with_exp(tlhs, &n_tlhs, scratch, n, v);
		subst_var_with_exp(trhs, &n_trhs, scratch, n, v);
	}
	/* Restore disguised variables: */
	for (j = 0; j < n_tlhs; j += 2)
		if (tlhs[j].kind == VARIABLE && tlhs[j].token.variable < 0)
			tlhs[j].token.variable = -tlhs[j].token.variable;
	for (j = 0; j < n_trhs; j += 2)
		if (trhs[j].kind == VARIABLE && trhs[j].token.variable < 0)
			trhs[j].token.variable = -trhs[j].token.variable;
	n_lhs[i] = n_tlhs;
	blt(lhs[i], tlhs, n_tlhs * sizeof(token_type));
	n_rhs[i] = n_trhs;
	blt(rhs[i], trhs, n_trhs * sizeof(token_type));
	simp_equation(i);
	return return_result(i);
}

/*
 * The simplify command.
 */
int
simplify_cmd(cp)
char	*cp;
{
	int		i, i1, j;
	int		k, k1;
	long		counter, counter_max, previous_solution_number[N_EQUATIONS];
	sign_array_type	sa_mark, sa_value;
	int		sign_flag = false, quick_flag = false, quickest_flag = false, symb = false, frac_flag = false;

	for (;; cp = skip_param(cp)) {
		if (strncasecmp(cp, "sign", 4) == 0) {
			sign_flag = true;
			continue;
		}
		if (strncasecmp(cp, "symbolic", 4) == 0) {
			symb = true;
			continue;
		}
		if (strcmp_tospace(cp, "quickest") == 0) {
			quickest_flag = true;
			continue;
		}
		if (strcmp_tospace(cp, "quick") == 0) {
			quick_flag = true;
			continue;
		}
		if (strncasecmp(cp, "fraction", 4) == 0) {
			frac_flag = true;
			continue;
		}
		break;
	}
	if (!get_range_eol(&cp, &i, &j)) {
		return false;
	}
	symb_flag = symb;
	for (; i <= j; i++) {
		if (n_lhs[i]) {
			if (quickest_flag) {
				simp_equation(i);
			} else {
				simpa_repeat_side(lhs[i], &n_lhs[i], quick_flag, frac_flag);
				simpa_repeat_side(rhs[i], &n_rhs[i], quick_flag, frac_flag);
			}
			if (!return_result(i)) {
				symb_flag = false;
				return false;
			}
			if (!sign_flag)
				continue;
			/* Now substitute all sign variables with +1 and -1. */
			CLEAR_ARRAY(previous_solution_number);
			CLEAR_ARRAY(sa_mark);
			for (k1 = 0; k1 < n_lhs[i]; k1 += 2) {
				if (lhs[i][k1].kind == VARIABLE && (lhs[i][k1].token.variable & VAR_MASK) == SIGN) {
					sa_mark[(lhs[i][k1].token.variable >> VAR_SHIFT) & SUBSCRIPT_MASK] = true;
				}
			}
			for (k1 = 0; k1 < n_rhs[i]; k1 += 2) {
				if (rhs[i][k1].kind == VARIABLE && (rhs[i][k1].token.variable & VAR_MASK) == SIGN) {
					sa_mark[(rhs[i][k1].token.variable >> VAR_SHIFT) & SUBSCRIPT_MASK] = true;
				}
			}
			for (k1 = 0, k = 0; k1 < ARR_CNT(sa_mark); k1++) {
				if (sa_mark[k1]) {
					k++;
				}
			}
			if (k == 0)
				continue;
			counter_max = (1L << k) - 1L;
			for (counter = 0; counter <= counter_max; counter++) {
				i1 = next_espace();
				copy_espace(i, i1);
				for (k1 = 0, k = 0; k1 < ARR_CNT(sa_mark); k1++) {
					if (sa_mark[k1]) {
						sa_value[k1] = (((1L << k) & counter) != 0);
						k++;
					}
				}
				for (k1 = 0; k1 < n_lhs[i1]; k1 += 2) {
					if (lhs[i1][k1].kind == VARIABLE && (lhs[i1][k1].token.variable & VAR_MASK) == SIGN) {
						if (sa_value[(lhs[i1][k1].token.variable >> VAR_SHIFT) & SUBSCRIPT_MASK]) {
							lhs[i1][k1].kind = CONSTANT;
							lhs[i1][k1].token.constant = -1.0;
						} else {
							lhs[i1][k1].kind = CONSTANT;
							lhs[i1][k1].token.constant = 1.0;
						}
					}
				}
				for (k1 = 0; k1 < n_rhs[i1]; k1 += 2) {
					if (rhs[i1][k1].kind == VARIABLE && (rhs[i1][k1].token.variable & VAR_MASK) == SIGN) {
						if (sa_value[(rhs[i1][k1].token.variable >> VAR_SHIFT) & SUBSCRIPT_MASK]) {
							rhs[i1][k1].kind = CONSTANT;
							rhs[i1][k1].token.constant = -1.0;
						} else {
							rhs[i1][k1].kind = CONSTANT;
							rhs[i1][k1].token.constant = 1.0;
						}
					}
				}
				for (k1 = 0, k = false; k1 < ARR_CNT(sa_mark); k1++) {
					if (sa_mark[k1]) {
						if (k) {
							fprintf(gfp, ", ");
						} else {
							fprintf(gfp, _("Solution number %ld with "), counter + 1);
						}
						list_var((long) SIGN + (((long) k1) << VAR_SHIFT), 0);
						fprintf(gfp, "%s = ", var_str);
						if (sa_value[k1]) {
							fprintf(gfp, "-1");
						} else {
							fprintf(gfp, "1");
						}
						k = true;
					}
				}
				if (k)
					fprintf(gfp, ":\n");
				if (quickest_flag) {
					simp_equation(i1);
				} else {
					simpa_repeat_side(lhs[i1], &n_lhs[i1], quick_flag, frac_flag);
					simpa_repeat_side(rhs[i1], &n_rhs[i1], quick_flag, frac_flag);
				}
				for (k1 = 0; k1 < ARR_CNT(previous_solution_number); k1++) {
					if (previous_solution_number[k1]) {
						if (compare_es(k1, i1)) {
							n_lhs[i1] = 0;
							n_rhs[i1] = 0;
							fprintf(gfp, _("is identical to solution number %ld.\n"), previous_solution_number[k1]);
							break;
						}
					}
				}
				if (n_lhs[i1]) {
					list_sub(i1);
					previous_solution_number[i1] = counter + 1;
				}
			}
		}
	}
	symb_flag = false;
	return true;
}

/*
 * The factor command.
 */
int
factor_cmd(cp)
char	*cp;
{
	int	i, j;
	int	i1;
	int	found;
	long	v;				/* Mathomatic variable */
	int	valid_range = false, power_flag = false;
	char	*cp_start;
#if	!LIBRARY
	char	*cp1;
	double	d;
	char	buf[MAX_CMD_LEN];
#endif

	cp_start = cp;
#if	!LIBRARY /* no integer factoring in library mode */
	if (strcmp_tospace(cp, "number") == 0) {
		cp = skip_param(cp);
	} else if (strcmp_tospace(cp, "numbers") == 0) {
		repeat_flag = true;
		cp = skip_param(cp);
	} else {
#endif
		if (strcmp_tospace(cp, "power") == 0) {
			power_flag = true;
			cp = skip_param(cp);
		}
		valid_range = get_range(&cp, &i, &j);
		if (!valid_range) {
#if	LIBRARY	/* be consistent */
			return false;
#else		/* be helpful */
			if (*cp == '-' || isdigit(*cp)) {
				printf(_("Factoring integers on command line instead:\n"));
			} else
				return false;
#endif
		}
#if	!LIBRARY
	}
	if (!valid_range) {
		do {
			if (*cp == '\0') {
retry:
				my_strlcpy(prompt_str, _("Enter integers to factor: "), sizeof(prompt_str));
				cp = get_string(buf, sizeof(buf));
				if (cp == NULL)
					return false;
				cp_start = cp;
			}
			if (*cp == '\0')
				return true;
			for (; *cp; ) {
				cp1 = cp;
				d = strtod(cp, &cp);
				if (cp == cp1) {
					error(_("Integer expected."));
					goto retry;
				}
				cp = skip_space(cp);
				if (*cp && !isdigit(*cp)) {
					input_column += (cp1 - cp_start);
					cp = parse_expr(tes, &n_tes, cp1);
					if (cp == NULL)
						goto retry;
					if (n_tes <= 0)
						return true;
					calc_simp(tes, &n_tes);
					if (n_tes != 1 || tes[0].kind != CONSTANT) {
						error(_("Integer expected."));
						goto retry;
					}
					d = tes[0].token.constant;
				}
				cp = skip_space(cp);
				if (!factor_one(d)) {
					error(_("Number too large to factor or not a non-zero integer."));
					goto retry;
				}
				display_unique();
				if (is_prime()) {
					fprintf(gfp, _("Prime number!\n"));
				}
			}
		} while (repeat_flag);
		return true;
	}
#endif
	if (power_flag) {
		if (extra_characters(cp))
			return false;
		for (i1 = i; i1 <= j; i1++) {
			if (n_lhs[i1]) {
				factor_power(lhs[i1], &n_lhs[i1]);
				do {
					simp_loop(lhs[i1], &n_lhs[i1]);
				} while (factor_power(lhs[i1], &n_lhs[i1]));
				if (n_rhs[i1]) {
					factor_power(rhs[i1], &n_rhs[i1]);
					do {
						simp_loop(rhs[i1], &n_rhs[i1]);
					} while (factor_power(rhs[i1], &n_rhs[i1]));
				}
				if (!return_result(i1))
					return false;
			}
		}
	} else {
		do {
			v = 0;
			if (*cp) {
				if ((cp = parse_var2(&v, cp)) == NULL) {
					return false;
				}
			}
			if (v) {
				found = false;
				for (i1 = i; i1 <= j; i1++) {
					if (var_in_equation(i1, v)) {
						found = true;
						break;
					}
				}
				if (!found) {
					warning(_("Variable not found."));
				}
			}
			for (i1 = i; i1 <= j; i1++) {
				if (n_lhs[i1]) {
					simpv_side(lhs[i1], &n_lhs[i1], v);
					if (n_rhs[i1]) {
						simpv_side(rhs[i1], &n_rhs[i1], v);
					}
				}
			}
		} while (*cp);
		for (i1 = i; i1 <= j; i1++) {
			if (n_lhs[i1]) {
				if (!return_result(i1))
					return false;
			}
		}
	}
	return true;
}

/*
 * The unfactor command.
 */
int
unfactor_cmd(cp)
char	*cp;
{
	int	i, j;
	int	quick_flag = false, fraction_flag = false, power_flag = false;

	for (;; cp = skip_param(cp)) {
		if (strncasecmp(cp, "quick", 4) == 0) {
			quick_flag = true;
			continue;
		}
		if (strncasecmp(cp, "fraction", 4) == 0 || strncasecmp(cp, "fully", 4) == 0) {
			fraction_flag = true;
			continue;
		}
		if (strncasecmp(cp, "power", 4) == 0) {
			power_flag = true;
			continue;
		}
		break;
	}
	if (!get_range_eol(&cp, &i, &j)) {
		return false;
	}
	partial_flag = !fraction_flag;
	if (power_flag) {
		for (; i <= j; i++) {
			if (n_lhs[i]) {
				if (quick_flag) {
					uf_power(lhs[i], &n_lhs[i]);
				} else {
					uf_allpower(lhs[i], &n_lhs[i]);
				}
				elim_loop(lhs[i], &n_lhs[i]);
				if (n_rhs[i]) {
					if (quick_flag) {
						uf_power(rhs[i], &n_rhs[i]);
					} else {
						uf_allpower(rhs[i], &n_rhs[i]);
					}
					elim_loop(rhs[i], &n_rhs[i]);
				}
				if (!return_result(i)) {
					partial_flag = true;
					return false;
				}
			}
		}
	} else {
		for (; i <= j; i++) {
			if (n_lhs[i]) {
				if (quick_flag) {
					uf_tsimp(lhs[i], &n_lhs[i]);
					if (n_rhs[i]) {
						uf_tsimp(rhs[i], &n_rhs[i]);
					}
				} else {
					uf_simp(lhs[i], &n_lhs[i]);
					if (n_rhs[i]) {
						uf_simp(rhs[i], &n_rhs[i]);
					}
				}
				if (!return_result(i)) {
					partial_flag = true;
					return false;
				}
			}
		}
	}
	partial_flag = true;
	return true;
}

/*
 * The fraction command.
 */
int
fraction_cmd(cp)
char	*cp;
{
	int	i, j;

	if (!get_range_eol(&cp, &i, &j)) {
		return false;
	}
	for (; i <= j; i++) {
		if (n_lhs[i]) {
			simple_frac_side(lhs[i], &n_lhs[i]);
			simple_frac_side(rhs[i], &n_rhs[i]);
			if (!return_result(i))
				return false;
		}
	}
	return true;
}

#if	!LIBRARY
/*
 * The quit command.
 */
int
quit_cmd(cp)
char	*cp;
{
	int	ev = 0;

	if (*cp) {
		ev = decstrtol(cp, &cp);
		if (extra_characters(cp))
			return false;
	}
	exit_program(ev);
	return false;		/* to placate the C compiler */
}
#endif

#if	!SECURE
/*
 * The read command.
 */
int
read_cmd(cp)
char	*cp;
{
	if (security_level >= 3) {
		error(_("Command disabled by security level."));
		return false;
	}
	return read_file(cp);
}

int
read_file(cp)
char	*cp;
{
	int	rv;
	FILE	*fp;
	char	buf[MAX_CMD_LEN];

	if (*cp == '\0') {
		error(_("No file name specified."));
		return false;
	}
	snprintf(buf, sizeof(buf), "%s.in", cp);
	fp = fopen(buf, "r");
	if (fp == NULL) {
		fp = fopen(cp, "r");
		if (fp == NULL) {
			perror(cp);
			error(_("Can't open file to read."));
			return false;
		}
	} else {
		cp = buf;
	}
	rv = read_sub(fp);
#if	!SILENT
	if (rv) {
		printf(_("Read operation aborted.\n"));
	} else {
		if (!quiet_mode) {
			printf(_("Finished reading file \"%s\".\n"), cp);
		}
	}
#endif
	fclose(fp);
	return(!rv);
}

/*
 * Read and process Mathomatic input from a file pointer.
 *
 * Return zero if no error, non-zero if aborted.
 */
int
read_sub(fp)
FILE	*fp;
{
	int	rv;
	jmp_buf	save_save;
	char	*cp;

	blt(save_save, jmp_save, sizeof(jmp_save));
	if ((rv = setjmp(jmp_save)) != 0) {	/* trap errors */
		clean_up();
		if (rv == 14) {
			error(_("Expression too large."));
		}
	} else {
		while ((cp = fgets((char *) tlhs, n_tokens * sizeof(token_type), fp)) != NULL) {
			if (!display_process(cp)) {
				longjmp(jmp_save, 3);	/* jump to the above error trap */
			}
		}
	}
	blt(jmp_save, save_save, sizeof(jmp_save));
	return rv;
}
#endif

#if	(UNIX || CYGWIN) && !SECURE && !LIBRARY
static int
edit_sub(cp)
char	*cp;
{
	char	cl[MAX_CMD_LEN];	/* command line */
	char	*cp1;

edit_again:
	cp1 = getenv("EDITOR");
	if (cp1 == NULL) {
#if	CYGWIN
		cp1 = "notepad";
#else
		cp1 = "nano";
#endif
		warning("EDITOR environment variable not set; using default editor.");
	}
	snprintf(cl, sizeof(cl), "%s %s", cp1, cp);
	if (shell_out(cl)) {
		error("Error executing editor, check EDITOR environment variable.");
		printf(_("Command line = \"%s\".\n"), cl);
		return false;
	}
	clear_all();
	if (!read_cmd(cp)) {
		if (pause_cmd(_("Prepare to rerun the editor"))) {
			goto edit_again;
		}
	}
	return true;
}

/*
 * The edit command.
 */
int
edit_cmd(cp)
char	*cp;
{
	FILE	*fp;
	int	fd;
	int	rv;
	char	tmp_file[PATH_MAX];

	if (security_level) {
		error(_("Command disabled by security level."));
		return false;
	}
	clean_up();	/* end any redirection */
	if (*cp == '\0') {
#if	CYGWIN
		my_strlcpy(tmp_file, "mathomatic.tmp", sizeof(tmp_file));
		fp = fopen(tmp_file, "w+");
		if (fp == NULL) {
			perror(tmp_file);
			error(_("Can't create temporary file."));
			return false;
		}
#else
		my_strlcpy(tmp_file, TMP_FILE, sizeof(tmp_file));
		fd = mkstemp(tmp_file);
		if (fd < 0 || (fp = fdopen(fd, "w+")) == NULL) {
			perror(tmp_file);
			error(_("Can't create temporary file."));
			return false;
		}
#endif
		gfp = fp;
		high_prec = true;
		list_cmd("all");
		high_prec = false;
		gfp = default_out;
		if (fclose(fp)) {
			rv = false;
			perror(tmp_file);
			error(_("Writing temporary file failed."));
		} else {
			rv = edit_sub(tmp_file);
		}
		unlink(tmp_file);
		return rv;
	} else {
		if (access(cp, R_OK | W_OK)) {
			error(_("You can only edit existing/writable files or all equations."));
			return false;
		}
		return edit_sub(cp);
	}
}
#endif

#if	!SECURE
/*
 * The save command.
 */
int
save_cmd(cp)
char	*cp;
{
	FILE	*fp;
	int	rv;

	if (security_level >= 2) {
		error(_("Command disabled by security level."));
		return false;
	}
	clean_up();	/* end any redirection */
	if (*cp == '\0') {
		error(_("No file name specified."));
		return false;
	}
#if	!SILENT
	if (access(cp, F_OK) == 0) {
		if (access(cp, W_OK)) {
			error(_("Specified save file is not writable."));
			return false;
		}
		snprintf(prompt_str, sizeof(prompt_str), _("File \"%s\" exists, overwrite (y/n)? "), cp);
		if (!get_yes_no()) {
			printf(_("Command aborted.\n"));
			return false;
		}
	}
#endif
	fp = fopen(cp, "w");
	if (fp == NULL) {
		perror(cp);
		error(_("Can't create specified save file."));
		return false;
	}
	gfp = fp;
	high_prec = true;
	rv = list_cmd("all");
	high_prec = false;
	gfp = default_out;
	if (fclose(fp)) {
		rv = false;
		perror(cp);
	}
	if (rv) {
#if	!SILENT
		printf(_("All equations saved in file \"%s\".\n"), cp);
#endif
	} else {
		error(_("Error encountered while saving equations."));
	}
	return rv;
}
#endif
