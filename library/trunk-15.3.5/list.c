/*
 * Mathomatic expression and equation display routines.
 * Color mode routines, too.
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

#define	EQUATE_STRING	" = "	/* string displayed between the LHS and RHS of equations */
#define MODULUS_STRING	" % "	/* string displayed for the modulus operator */

static int	flist_sub();
static int	flist_recurse();

/* ANSI terminal color code array for 8 color ANSI; we don't use black or white */
/* because the background may be the same color, so there are only 6 colors here. */
static int	carray[] = {
	32,	/* must be green (default color) */
	33,	/* must be yellow (for warnings) */
	31,	/* must be red (for errors) */
	35,	/* magenta */
	34,	/* blue */
	36	/* cyan */
};

/* Bright HTML color array. */
/* Used when HTML output and "set color" and "set bold" options are enabled. */
/* Good looking with a dark background. */
static char	*bright_html_carray[] = {
	"#00FF00",	/* must be bright green (default color) */
	"#FFFF00",	/* must be bright yellow (for warnings) */
	"#FF0000",	/* must be bright red (for errors) */
	"#FF9000",
	"#FF00FF",
	"#00FFFF",
	"#0000FF"
};

/* Dim HTML color array for color HTML output. */
/* Used for HTML output with "set color" and "set no bold" options. */
/* Good looking with a white background. */
static char	*html_carray[] = {
	"green",
	"olive",
	"red",
	"maroon",
	"purple",
	"teal",
	"navy"
};

/* global variables for flist_equation() below */
static int	cur_line;	/* current line */
static int	cur_pos;	/* current position in the current line on the screen */

/*
 * Reset terminal attributes function.
 * Turn color off if color mode is on.
 * Subsequent output will have no color until the next call to set_color().
 */
void
reset_attr()
{
	fflush(NULL);	/* flush all output */
	if (color_flag && cur_color >= 0) {
		if (html_flag) {
			printf("</font>");
		} else {
			printf("\033[0m");
		}
		fflush(stdout);	/* flush standard output */
	}
	cur_color = -1;
}

/*
 * Set the current color on the display.
 * Subsequent output will have the color in carray[color % ARR_CNT(carray)].
 * Range is 0 to INT_MAX.
 */
void
set_color(color)
int	color;
{
	if (gfp != stdout)
		return;
	if (cur_color == color)
		return;
	if (color_flag) {
		if (html_flag) {
			if (cur_color >= 0) {
				printf("</font>");
			}
			if (bold_colors) {
				printf("<font color=\"%s\">", bright_html_carray[color % ARR_CNT(bright_html_carray)]);
			} else {
				printf("<font color=\"%s\">", html_carray[color % ARR_CNT(html_carray)]);
			}
		} else {
			printf("\033[%d;%dm", bold_colors, carray[color % ARR_CNT(carray)]);
		}
		cur_color = color;
	}
}

/*
 * Set normal text color for subsequent output.
 * Currently, the normal color (0) is green.
 */
void
default_color()
{
	set_color(0);
	fflush(stdout);
}

/*
 * Trim the trailing zeros from a string, after the decimal point.
 */
static void
trim_zeros(buf)
char	*buf;
{
	int	j;

	j = strlen(buf) - 1;
	for (; j >= 0; j--) {
		if (buf[j] == '0')
			continue;
		if (buf[j] == '.') {
			if (buf[j+1])
				buf[j+2] = '\0';
		} else {
			break;
		}
	}
}

/*
 * Display the expression or equation stored in equation space "n" in single-line format.
 *
 * Return length (number of screen columns) of output line.
 */
int
list1_sub(n, export_flag)
int	n;		/* equation space number */
int	export_flag;	/* non-zero for exportable format (readable by other math programs) */
			/* 1 for Maxima, 2 for other, 3 for Gnuplot, 4 for hexadecimal */
{
	int	len = 0;

	if (n_lhs[n] <= 0)
		return 0;
	if ((export_flag == 0 || export_flag == 4) && !high_prec) {
		len += fprintf(gfp, "#%d: ", n + 1);
	}
	len += list_proc(lhs[n], n_lhs[n], export_flag);
	if (n_rhs[n]) {
		len += fprintf(gfp, EQUATE_STRING);
		len += list_proc(rhs[n], n_rhs[n], export_flag);
	}
	if (export_flag == 1) {
		len += fprintf(gfp, ";");
	}
#if	CYGWIN
	fprintf(gfp, "\r\n");	/* might be redirecting to a Microsoft text file */
#else
	fprintf(gfp, "\n");
#endif
	return len;
}

/*
 * Display the expression or equation stored in equation space "n".
 *
 * Return the total width of the output (number of screen columns)
 * or zero on failure.
 */
int
list_sub(n)
int	n;	/* equation space number */
{
	if (n_lhs[n] <= 0)
		return 0;
	make_fractions_and_group(n);
	if (factor_int_flag) {
		factor_int_sub(n);
	}
	if (display2d) {
		/* display in fraction format */
		return flist_equation(n);
	} else {
		/* display in single-line format */
		return list1_sub(n, false);
	}
}

#if	!SILENT
void
list_debug(level, p1, n1, p2, n2)
int		level;
token_type	*p1;
int		n1;
token_type	*p2;
int		n2;
{
	if (debug_level >= level) {
		fprintf(gfp, _("level %d: "), level);
		list_proc(p1, n1, false);
		if (p2 && n2 > 0) {
			fprintf(gfp, EQUATE_STRING);
			list_proc(p2, n2, false);
		}
		fprintf(gfp, "\n");
	}
}
#endif

/*
 * Return the allocated string name of the given Mathomatic variable,
 * or NULL if none.
 *
 * Does not return the actual variable name, use list_var() for that.
 */
char *
var_name(v)
long	v;	/* Mathomatic variable */
{
	char	*cp = NULL;
	long	l;

	l = (v & VAR_MASK) - VAR_OFFSET;
	if (l >= 0 && l < MAX_VAR_NAMES) {
		cp = var_names[l];
	}
	return cp;
}

/*
 * Convert the passed Mathomatic variable to an ASCII variable name.
 * The ASCII variable name is stored in the global var_str[].
 *
 * Return the length of the variable name string in var_str[].
 * Nothing is displayed.
 *
 * If (lang_code == 0), use standard Mathomatic format.
 * If (lang_code > 0), make variable compatible with the language defined in the enumeration language_list defined in am.h
 * If (lang_code < 0), create an exportable variable name: -1 for Maxima, -2 for other, -3 for Gnuplot, -4 for hexadecimal.
 */
int
list_var(v, lang_code)
long	v;		/* variable to convert */
int	lang_code;	/* language code */
{
	int		j;
	int		from_memory = false;
	char		*cp = NULL;

	var_str[0] = '\0';
	switch (v & VAR_MASK) {
	case V_NULL:
		return(strlen(var_str));
	case SIGN:
		cp = "sign";
		break;
	case IMAGINARY:
		switch (lang_code) {
		case -3:
			cp = "{0,1}";
			break;
		case -2:
			cp = "i";
			break;
		case -1:
			cp = "%i";
			break;
		case 0:
		case -4:
			cp = "i#";
			break;
		case PYTHON:
			cp = "1j";
			break;
		default:
			cp = "1.0i";
			break;
		}
		break;
	case V_E:
		switch (lang_code) {
		case -3:
			cp = "exp(1.0)";
			break;
		case -1:
			cp = "%e";
			break;
		case 0:
		case -4:
			cp = "e#";
			break;
		case C:
			cp ="M_E";
			break;
		case JAVA:
			cp = "Math.E";
			break;
		case PYTHON:
			cp = "math.e";
			break;
		default:
			cp = "e";
			break;
		}
		break;
	case V_PI:
		switch (lang_code) {
		case -1:
			cp = "%pi";
			break;
		case 0:
		case -4:
			cp = "pi#";
			break;
		case C:
			cp = "M_PI";
			break;
		case JAVA:
			cp = "Math.PI";
			break;
		case PYTHON:
			cp = "math.pi";
			break;
		default:
			cp = "pi";
			break;
		}
		break;
	case MATCH_ANY:
		cp = "all";
		break;
	default:
		cp = var_name(v);
		from_memory = true;
		break;
	}
	if (cp) {
		j = (v >> VAR_SHIFT) & SUBSCRIPT_MASK;
		if (j) {	/* for "sign" variables */
			snprintf(var_str, sizeof(var_str), "%s%d", cp, j - 1);
		} else {
			my_strlcpy(var_str, cp, sizeof(var_str));
		}
	} else {
		my_strlcpy(var_str, "bad_variable", sizeof(var_str));
	}
/* Make valid C type variable if exporting or generating code: */
	if (from_memory && lang_code != 0 && lang_code != -4) {
		for (j = 0; var_str[j]; j++) {
			if (var_str[j] != '_' && !isalnum(var_str[j])) {
				var_str[j] = '_';
			}
		}
	}
	return(strlen(var_str));
}

/*
 * Display an expression in single-line format.
 * Use color if color mode is set.
 *
 * Return number of characters output (excluding escape sequences).
 */
int
list_proc(p1, n, export_flag)
token_type	*p1;		/* expression pointer */
int		n;		/* length of expression */
int		export_flag;	/* flag for exportable format (usually false) */
				/* 1 for Maxima, 2 for other, 3 for Gnuplot, 4 for hexadecimal */
{
	return list_string_sub(p1, n, true, NULL, export_flag);
}

/*
 * Store the specified equation in a text string.
 * String should be freed with free() when done.
 *
 * Return string or NULL if error.
 */
char *
list_equation(n, export_flag)
int	n;		/* equation space number */
int	export_flag;	/* flag for exportable format (usually false) */
{
	int	len;
	char	*cp;

	if (n_lhs[n] <= 0) {
		return NULL;
	}
	len = list_string(lhs[n], n_lhs[n], NULL, export_flag);
	if (n_rhs[n]) {
		len += strlen(EQUATE_STRING);
		len += list_string(rhs[n], n_rhs[n], NULL, export_flag);
	}
	len += 2;	/* for possible semicolon and terminating null character */
	cp = (char *) malloc(len);
	if (cp == NULL) {
		error(_("Out of memory (can't malloc(3))."));
		return NULL;
	}
	list_string(lhs[n], n_lhs[n], cp, export_flag);
	if (n_rhs[n]) {
		strcat(cp, EQUATE_STRING);
		list_string(rhs[n], n_rhs[n], &cp[strlen(cp)], export_flag);
	}
	if (export_flag == 1) {
		strcat(cp, ";");
	}
	return cp;
}

/*
 * Store an expression in a text string.
 * String should be freed with free() when done.
 *
 * Return string or NULL if error.
 */
char *
list_expression(p1, n, export_flag)
token_type	*p1;		/* expression pointer */
int		n;		/* length of expression */
int		export_flag;
{
	int	len;
	char	*cp;

	if (n <= 0) {
		return NULL;
	}
	len = list_string(p1, n, NULL, export_flag);
	len++;	/* for terminating null character */
	cp = (char *) malloc(len);
	if (cp == NULL) {
		error(_("Out of memory (can't malloc(3))."));
		return NULL;
	}
	list_string(p1, n, cp, export_flag);
	return cp;
}

/*
 * Convert an expression to a text string and store in "string" if
 * "string" is not NULL.  "string" need not be initialized,
 * but must be long enough to contain the expression.
 * To find the storage size needed, call with "string" set to NULL first.
 *
 * Return length (number of characters).
 */
int
list_string(p1, n, string, export_flag)
token_type	*p1;		/* expression pointer */
int		n;		/* length of expression */
char		*string;	/* buffer to save output to or NULL pointer */
int		export_flag;
{
	return list_string_sub(p1, n, false, string, export_flag);
}

int
list_string_sub(p1, n, outflag, string, export_flag)
token_type	*p1;		/* expression pointer */
int		n;		/* length of expression */
int		outflag;	/* if true, output to gfp */
char		*string;	/* buffer to save output to or NULL pointer */
int		export_flag;	/* flag for exportable format (usually false) */
				/* 1 for Maxima, 2 for other, 3 for Gnuplot, 4 for hexadecimal */
{
#define	APPEND(str)	{ if (string) { strcpy(&string[len], str); } if (outflag) { fprintf(gfp, "%s", str); } len += strlen(str); }

	int	i, j, k, i1;
	int	min1;
	int	cur_level;
	char	*cp;
	int	len = 0;
	char	buf[500], buf2[500];
	int	export_precision;
	int	cflag, power_flag;

	cflag = (outflag && (export_flag == 0 || export_flag == 4));
	if (string)
		string[0] = '\0';
	if (high_prec)
		export_precision = 20;
	else
		export_precision = DBL_DIG;
	cur_level = min1 = min_level(p1, n);
	for (i = 0; i < n; i++) {
		power_flag = false;
#if	1
		if (export_flag == 0 && !high_prec) {
			for (j = i - 1; j <= (i + 1); j++) {
				if ((j - 1) >= 0 && (j + 1) < n
				    && p1[j].kind == OPERATOR && (p1[j].token.operatr == POWER || p1[j].token.operatr == FACTORIAL)
				    && p1[j-1].level == p1[j].level
				    && p1[j+1].level == p1[j].level
				    && ((j + 2) >= n || p1[j+2].level != (p1[j].level - 1) || (p1[j+2].token.operatr < POWER))
				    && ((j - 2) < 0 || p1[j-2].level != (p1[j].level - 1) || (p1[j-2].token.operatr < POWER))) {
					power_flag = true;
					break;
				}
			}
		}
#endif
		j = cur_level - p1[i].level;
		if (power_flag)
			k = abs(j) - 1;
		else
			k = abs(j);
		for (i1 = 1; i1 <= k; i1++) {
			if (j > 0) {
				cur_level--;
				APPEND(")");
				if (cflag)
					set_color(cur_level-min1);
			} else {
				cur_level++;
				if (cflag)
					set_color(cur_level-min1);
				APPEND("(");
			}
		}
		switch (p1[i].kind) {
		case CONSTANT:
			if (p1[i].token.constant == 0.0) {
				p1[i].token.constant = 0.0; /* fix -0 */
			}
			if (export_flag == 4) {
				snprintf(buf, sizeof(buf), "%a", p1[i].token.constant);
			} else if (export_flag == 3) {
				snprintf(buf, sizeof(buf), "%#.*g", DBL_DIG, p1[i].token.constant);
				trim_zeros(buf);
			} else if (export_flag || high_prec) {
				snprintf(buf, sizeof(buf), "%.*g", export_precision, p1[i].token.constant);
			} else if (finance_option) {
				snprintf(buf, sizeof(buf), "%'.*f", finance_option, p1[i].token.constant);
			} else {
				if (p1[i].token.constant < 0.0 && (i + 1) < n && p1[i+1].level == p1[i].level
				    && (p1[i+1].token.operatr >= POWER)) {
					snprintf(buf, sizeof(buf), "(%.*g)", precision, p1[i].token.constant);
				} else {
					snprintf(buf, sizeof(buf), "%.*g", precision, p1[i].token.constant);
				}
				APPEND(buf);
				break;
			}
			if (p1[i].token.constant < 0.0) {
				snprintf(buf2, sizeof(buf2), "(%s)", buf);
				APPEND(buf2);
			} else {
				APPEND(buf);
			}
			break;
		case VARIABLE:
			list_var(p1[i].token.variable, 0 - export_flag);
			APPEND(var_str);
			break;
		case OPERATOR:
			cp = _("(unknown operator)");
			switch (p1[i].token.operatr) {
			case PLUS:
				cp = " + ";
				break;
			case MINUS:
				cp = " - ";
				break;
			case TIMES:
				cp = "*";
				break;
			case DIVIDE:
				cp = "/";
				break;
			case IDIVIDE:
				cp = "//";
				break;
			case MODULUS:
				cp = MODULUS_STRING;
				break;
			case POWER:
				if (power_starstar || export_flag == 3) {
					cp = "**";
				} else {
					cp = "^";
				}
				break;
			case FACTORIAL:
				cp = "!";
				i++;
				break;
			}
			APPEND(cp);
			break;
		}
	}
	for (j = cur_level - min1; j > 0;) {
		APPEND(")");
		j--;
		if (cflag)
			set_color(j);
	}
	return len;
}

/*
 * Return true if expression is a valid integer expression for
 * list_code().
 */
int
int_expr(p1, n)
token_type	*p1;		/* expression pointer */
int		n;		/* length of expression */
{
	int	i;

	for (i = 0; i < n; i++) {
		switch (p1[i].kind) {
		case CONSTANT:
			if (fmod(p1[i].token.constant, 1.0) != 0.0) {
				return false;
			}
			break;
		case VARIABLE:
			if (p1[i].token.variable < IMAGINARY) {
				return false;
			}
			break;
		case OPERATOR:
			break;
		}
	}
	return true;
}

/*
 * Display an equation space as C, Java, or Python code.
 *
 * Return length of output (number of characters).
 */
int
list_c_equation(en, language, int_flag)
int			en;		/* equation space number */
enum language_list	language;
int			int_flag;	/* integer arithmetic flag */
{
	int	len = 0;

	if (n_lhs[en] <= 0)
		return 0;
	len += list_code(lhs[en], &n_lhs[en], language, int_flag);
	if (n_rhs[en]) {
		len += fprintf(gfp, " = ");
		len += list_code(rhs[en], &n_rhs[en], language, int_flag);
	}
	switch (language) {
	case C:
	case JAVA:
		len += fprintf(gfp, ";");
		break;
	default:
		break;
	}
	fprintf(gfp, "\n");
	return len;
}

/*
 * Output C, Java, or Python code for an expression.
 * Expression might be modified by this function, though it remains equivalent.
 *
 * Return length of output (number of characters).
 */
int
list_code(equation, np, language, int_flag)
token_type		*equation;	/* equation side pointer */
int			*np;		/* pointer to length of equation side */
enum language_list	language;	/* see enumeration language_list in am.h */
int			int_flag;	/* integer arithmetic flag, should work with any language */
{
	int	i, j, k, i1, i2;
	int	min1;
	int	cur_level;
	char	*cp;
	char	buf[500];
	int	len = 0;

	min1 = min_level(equation, *np);
	if (*np > 1)
		min1--;
	cur_level = min1;
	for (i = 0; i < *np; i++) {
		j = cur_level - equation[i].level;
		k = abs(j);
		for (i1 = 1; i1 <= k; i1++) {
			if (j > 0) {
				cur_level--;
				len += fprintf(gfp, ")");
			} else {
				cur_level++;
				for (i2 = i + 1; i2 < *np && equation[i2].level >= cur_level; i2 += 2) {
					if (equation[i2].level == cur_level) {
						switch (equation[i2].token.operatr) {
						case POWER:
							if (equation[i2-1].level == cur_level
							    && equation[i2+1].level == cur_level
							    && equation[i2+1].kind == CONSTANT
							    && equation[i2+1].token.constant == 2.0) {
								equation[i2].token.operatr = TIMES;
								equation[i2+1] = equation[i2-1];
							} else {
								if (!int_flag) {
									switch (language) {
									case C:
										len += fprintf(gfp, "pow");
							 			break;
									case JAVA:
										len += fprintf(gfp, "Math.pow");
										break;
									default:
										break;
									}
								}
							}
							break;
						case FACTORIAL:
							len += fprintf(gfp, "fact");
							break;
						}
						break;
					}
				}
				len += fprintf(gfp, "(");
			}
		}
		switch (equation[i].kind) {
		case CONSTANT:
			if (equation[i].token.constant == 0.0) {
				equation[i].token.constant = 0.0; /* fix -0 */
			}
			if (int_flag) {
				snprintf(buf, sizeof(buf), "%.0f", equation[i].token.constant);
			} else {
				snprintf(buf, sizeof(buf), "%#.*g", DBL_DIG, equation[i].token.constant);
				trim_zeros(buf);
			}
/* Here we will need to parenthesize negative numbers to make -2**x work the same with Python: */
			if (equation[i].token.constant < 0) {
				len += fprintf(gfp, "(%s)", buf);
			} else {
				len += fprintf(gfp, "%s", buf);
			}
			break;
		case VARIABLE:
		  	if (int_flag && (language == C || language == JAVA) && equation[i].token.variable == IMAGINARY) {
				len += fprintf(gfp, "1i");
			} else {
				list_var(equation[i].token.variable, language);
				len += fprintf(gfp, "%s", var_str);
			}
			break;
		case OPERATOR:
			cp = _("(unknown operator)");
			switch (equation[i].token.operatr) {
			case PLUS:
				cp = " + ";
				break;
			case MINUS:
				cp = " - ";
				break;
			case TIMES:
				cp = "*";
				break;
			case DIVIDE:
			case IDIVIDE:
				cp = "/";
				break;
			case MODULUS:
				cp = MODULUS_STRING;
				break;
			case POWER:
				if (int_flag || language == PYTHON) {
					cp = "**";
				} else {
					cp = ", ";
				}
				break;
			case FACTORIAL:
				cp = "";
				i++;
				break;
			}
			len += fprintf(gfp, "%s", cp);
			break;
		}
	}
	for (j = cur_level - min1; j > 0; j--) {
		len += fprintf(gfp, ")");
	}
	return len;
}

/*
 * Display an equation space in 2D multi-line fraction format.
 * Use color if color mode is set.
 *
 * The equation sides must first be basically simplified and prepared by fractions_and_group(),
 * for proper display.
 *
 * Return the total width of the output (that is, the required number of screen columns)
 * or zero on failure.
 */
int
flist_equation(n)
int	n;	/* equation space number */
{
	int	sind;
	char	buf[50];
	int	len, len2, len3, width;
	int	pos;
	int	high = 0, low = 0;
	int	max_line = 0, min_line = 0;
	int	max2_line = 0, min2_line = 0;

	if (n_lhs[n] <= 0)
		return 0;
	len = snprintf(buf, sizeof(buf), "#%d: ", n + 1);
	cur_line = 0;
	cur_pos = 0;
	sind = n_rhs[n];
	len += flist_sub(lhs[n], n_lhs[n], false, 0, &max_line, &min_line);
	if (n_rhs[n]) {
		len += strlen(EQUATE_STRING);
make_smaller:
		len2 = flist_sub(rhs[n], sind, false, 0, &high, &low);
		if (screen_columns && gfp == stdout && (len + len2) >= screen_columns && sind > 0) {
			for (sind--; sind > 0; sind--) {
				if (rhs[n][sind].level == 1 && rhs[n][sind].kind == OPERATOR) {
					switch (rhs[n][sind].token.operatr) {
					case PLUS:
					case MINUS:
					case MODULUS:
						goto make_smaller;
					}
				}
			}
			goto make_smaller;
		}
		if (high > max_line)
			max_line = high;
		if (low < min_line)
			min_line = low;
		len3 = flist_sub(&rhs[n][sind], n_rhs[n] - sind, false, 0, &max2_line, &min2_line);
	} else {
		len2 = 0;
		len3 = 0;
	}
	width = max(len + len2, len3);
	if (screen_columns && gfp == stdout && width >= screen_columns) {
		/* output too wide to fit screen, output in single-line format */
		width = list1_sub(n, false);
#if	CYGWIN
		fprintf(gfp, "\r\n");	/* Be consistent with list1_sub() output. */
#else
		fprintf(gfp, "\n");
#endif
		return width;
	}
	fprintf(gfp, "\n");
	for (cur_line = max_line; cur_line >= min_line; cur_line--) {
		pos = cur_pos = 0;
		if (cur_line == 0) {
			cur_pos += fprintf(gfp, "%s", buf);
		}
		pos += strlen(buf);
		pos += flist_sub(lhs[n], n_lhs[n], true, pos, &high, &low);
		if (n_rhs[n]) {
			if (cur_line == 0) {
				cur_pos += fprintf(gfp, "%s", EQUATE_STRING);
			}
			pos += strlen(EQUATE_STRING);
			pos += flist_sub(rhs[n], sind, true, pos, &high, &low);
		}
		fprintf(gfp, "\n");
	}
	if (sind < n_rhs[n]) {
/* output second half of split equation that was too wide to display on the screen without splitting */
		fprintf(gfp, "\n");
		for (cur_line = max2_line; cur_line >= min2_line; cur_line--) {
			cur_pos = 0;
			flist_sub(&rhs[n][sind], n_rhs[n] - sind, true, 0, &high, &low);
			fprintf(gfp, "\n");
		}
	}
	fprintf(gfp, "\n");
	return width;
}

/*
 * Display a line of an expression in 2D fraction format if "out_flag" is true.
 * The number of the line to output is stored in the global variable cur_line.
 * 0 is the middle line, lines above are positive, lines below are negative.
 * Use color if available.
 *
 * This function is for internal use only, and not to be called, except by flist_equation().
 *
 * Return the width of the expression (that is, the required number of screen columns).
 */
static int
flist_sub(p1, n, out_flag, pos, highp, lowp)
token_type	*p1;		/* expression pointer */
int		n;		/* length of expression */
int		out_flag;
int		pos;
int		*highp, *lowp;
{
	int	i;

	i = flist_recurse(p1, n, out_flag, 0, pos, 1, highp, lowp);
	if (out_flag) {
		default_color();
	}
	return i;
}

static int
flist_recurse(p1, n, out_flag, line, pos, cur_level, highp, lowp)
token_type	*p1;
int		n;
int		out_flag;
int		line;
int		pos;
int		cur_level;
int		*highp, *lowp;
{
	int	i, j, k, i1;
	int	l1, l2;
	int	ii;
	int	stop_at;
	int	div_loc;
	int	len_div;
	int	level;
	int	start_level;
	int	oflag, html_out, power_flag;
	int	len = 0, len1, len2;
	int	high, low;
	char	buf[500];
	char	*cp;

	start_level = cur_level;
	*highp = line;
	*lowp = line;
	if (n <= 0) {
		return 0;
	}
	oflag = (out_flag && line == cur_line);
	html_out = (html_flag && gfp == stdout);
	if (oflag) {
		set_color(cur_level-1);
		for (; cur_pos < pos; cur_pos++) {
			fprintf(gfp, " ");
		}
	}
	ii = 0;
check_again:
	stop_at = n;
	div_loc = -1;
	for (i = ii; i < n; i++) {
		if (p1[i].kind == OPERATOR && p1[i].token.operatr == DIVIDE) {
			level = p1[i].level;
			for (j = i - 2; j > 0; j -= 2) {
				if (p1[j].level < level)
					break;
			}
			j++;
			if (div_loc < 0) {
				div_loc = i;
				stop_at = j;
			} else {
				if (j < stop_at) {
					div_loc = i;
					stop_at = j;
				} else if (j == stop_at) {
					if (level < p1[div_loc].level)
						div_loc = i;
				}
			}
		}
	}
	for (i = ii; i < n; i++) {
		power_flag = false;
		if (i == stop_at) {
			j = cur_level - p1[div_loc].level;
			k = abs(j) - 1;
		} else {
#if	1
			for (j = i - 1; j <= (i + 1); j++) {
				if ((j - 1) >= ii && (j + 1) < n
				    && p1[j].kind == OPERATOR && (p1[j].token.operatr == POWER || p1[j].token.operatr == FACTORIAL)
				    && p1[j-1].level == p1[j].level
				    && p1[j+1].level == p1[j].level
				    && ((j + 2) >= n || p1[j+2].level != (p1[j].level - 1) || (p1[j+2].token.operatr < POWER))
				    && ((j - 2) < ii || p1[j-2].level != (p1[j].level - 1) || (p1[j-2].token.operatr < POWER))) {
					power_flag = true;
					break;
				}
			}
#endif
			j = cur_level - p1[i].level;
			if (power_flag)
				k = abs(j) - 1;
			else
				k = abs(j);
		}
		for (i1 = 1; i1 <= k; i1++) {
			if (j > 0) {
				cur_level--;
				len++;
				if (oflag) {
					fprintf(gfp, ")");
					set_color(cur_level-1);
				}
			} else {
				cur_level++;
				len++;
				if (oflag) {
					set_color(cur_level-1);
					fprintf(gfp, "(");
				}
			}
		}
		if (i == stop_at) {
			level = p1[div_loc].level;
			len1 = flist_recurse(&p1[stop_at], div_loc - stop_at, false, line + 1, pos + len, level, &high, &low);
			l1 = (2 * (line + 1)) - low;
			for (j = div_loc + 2; j < n; j += 2) {
				if (p1[j].level <= level)
					break;
			}
			len2 = flist_recurse(&p1[div_loc+1], j - (div_loc + 1), false, line - 1, pos + len, level, &high, &low);
			l2 = (2 * (line - 1)) - high;
			ii = j;
			len_div = max(len1, len2);
			j = 0;
			if (len1 < len_div) {
				j = (len_div - len1) / 2;
			}
			flist_recurse(&p1[stop_at], div_loc - stop_at, out_flag, l1, pos + len + j, level, &high, &low);
			if (high > *highp)
				*highp = high;
			if (low < *lowp)
				*lowp = low;
			if (oflag) {
				/* display fraction bar */
				set_color(level-1);
				for (j = 0; j < len_div; j++) {
					fprintf(gfp, "-");
				}
				set_color(cur_level-1);
			}
			j = 0;
			if (len2 < len_div) {
				j = (len_div - len2) / 2;
			}
			flist_recurse(&p1[div_loc+1], ii - (div_loc + 1), out_flag, l2, pos + len + j, level, &high, &low);
			if (high > *highp)
				*highp = high;
			if (low < *lowp)
				*lowp = low;
			len += len_div;
			goto check_again;
		}
		switch (p1[i].kind) {
		case CONSTANT:
			if (p1[i].token.constant == 0.0) {
				p1[i].token.constant = 0.0; /* fix -0 */
			}
			if (p1[i].token.constant == -1.0 && (i == 0 || p1[i-1].level < p1[i].level)
			    && (i + 1) < n && p1[i].level == p1[i+1].level && p1[i+1].token.operatr == TIMES) {
				i++;
				len += snprintf(buf, sizeof(buf), "-");
			} else if (finance_option) {
				if (p1[i].token.constant < 0.0) {
					len += snprintf(buf, sizeof(buf), "(%'.*f)", finance_option, p1[i].token.constant);
				} else {
					len += snprintf(buf, sizeof(buf), "%'.*f", finance_option, p1[i].token.constant);
				}
			} else {
				if (p1[i].token.constant < 0.0 && (i + 1) < n && p1[i+1].level == p1[i].level
				    && (p1[i+1].token.operatr >= POWER)) {
					len += snprintf(buf, sizeof(buf), "(%.*g)", precision, p1[i].token.constant);
				} else {
					len += snprintf(buf, sizeof(buf), "%.*g", precision, p1[i].token.constant);
				}
			}
			if (oflag)
				fprintf(gfp, "%s", buf);
			break;
		case VARIABLE:
			len += list_var(p1[i].token.variable, 0);
			if (oflag)
				fprintf(gfp, "%s", var_str);
			break;
		case OPERATOR:
/* "len" must be incremented here by the number of columns the display of the operator takes */
			switch (p1[i].token.operatr) {
			case PLUS:
				cp = " + ";
				len += 3;
				break;
			case MINUS:
				if (html_out) {
					cp = " &minus; ";
				} else {
					cp = " - ";
				}
				len += 3;
				break;
			case TIMES:
				if (html_out) {
					cp = "&middot;";
				} else {
					cp = "*";
				}
				len++;
				break;
			case DIVIDE:
				cp = "/";
				len++;
				break;
			case IDIVIDE:
				cp = "//";
				len += 2;
				break;
			case MODULUS:
				cp = MODULUS_STRING;
				len += strlen(cp);
				break;
			case POWER:
#if	0
				if (html_out && power_flag
				    && p1[i-1].level == p1[i].level
				    && p1[i+1].level == p1[i].level
				    && p1[i+1].kind == CONSTANT) {
					if (p1[i+1].token.constant == 2.0) {
						cp = "&sup2;";
						len++;
						i++;
						break;
					}
					if (p1[i+1].token.constant == 3.0) {
						cp = "&sup3;";
						len++;
						i++;
						break;
					}
				}
#endif
				if (power_starstar) {
					cp = "**";
					len += 2;
				} else {
					cp = "^";
					len++;
				}
				break;
			case FACTORIAL:
				cp = "!";
				len++;
				i++;
				break;
			default:
				cp = _("(unknown operator)");
				len += strlen(cp);
				break;
			}
			if (oflag)
				fprintf(gfp, "%s", cp);
			break;
		}
	}
	for (j = cur_level - start_level; j > 0;) {
		cur_level--;
		len++;
		j--;
		if (oflag) {
			fprintf(gfp, ")");
			if (j > 0)
				set_color(cur_level-1);
		}
	}
	if (oflag)
		cur_pos += len;
	return len;
}
