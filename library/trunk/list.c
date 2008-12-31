/*
 * Mathomatic expression and equation display routines.
 * Color mode routines, too.
 *
 * Copyright (C) 1987-2008 George Gesslein II.
 */

#include "includes.h"

#define	EQUATE_STRING	" = "	/* string displayed between the LHS and RHS of equations */
#define MODULUS_STRING	" % "	/* string displayed for the modulus operator */

#define	APPEND(str)	{ if (string) { strcpy(&string[len], str); } len += strlen(str); }

static int	flist();
static int	flist_recurse();

/* ANSI terminal color code array for 8 color ANSI, we don't use black or white */
/* because the background may be the same color. */
static int	carray[] = {
	32,	/* green (default color) */
	33,	/* yellow */
	31,	/* red */
	35,	/* magenta */
	34,	/* blue */
	36	/* cyan */
};

/* Bright HTML color array. */
/* Used when HTML output and "set color" and "set bold" options are enabled. */
/* Good looking with a dark background. */
static char	*bright_html_carray[] = {
	"#00FF00",	/* bright green (default color) */
	"#FFFF00",
	"#FF0000",
	"#FF9000",
	"#FF00FF",
	"#00FFFF",
	"#0000FF"
};

/* Dim HTML color array for color HTML output. */
/* Good looking with a white background. */
static char	*html_carray[] = {
	"green",	/* green (default color) */
	"olive",
	"red",
	"maroon",
	"purple",
	"teal",
	"navy"
};

/* global variables for flist() below */
static int	cur_line;	/* current line */
static int	cur_pos;	/* current position in the current line on the screen */

/*
 * Reset terminal attributes function.
 * Turn color off if color mode is on.
 * Subsequent output will have no color.
 */
void
reset_attr()
{
	cur_color = -1;
	if (color_flag) {
		if (html_flag) {
			printf("</font>");
		} else {
			printf("\033[0m");
		}
	}
	fflush(NULL);	/* flush all output */
}

/*
 * Set the current color on the display.
 * Subsequent output will have the color in carray[color].
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
				printf("<font color=\"%s\">", bright_html_carray[color%ARR_CNT(bright_html_carray)]);
			} else {
				printf("<font color=\"%s\">", html_carray[color%ARR_CNT(html_carray)]);
			}
		} else {
			printf("\033[%d;%dm", bold_colors, carray[color%ARR_CNT(carray)]);
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
}

/*
 * Display the expression or equation stored in equation space "n" in single-line format.
 * The equation space is not modified.
 *
 * Return length (number of screen columns) of output line.
 */
int
list1_sub(n, export_flag)
int	n;		/* equation space number */
int	export_flag;	/* non-zero for exportable format (readable by other math programs) */
			/* 1 for Maxima, 2 for other */
{
	int	len = 0;

	if (n_lhs[n] <= 0)
		return 0;
	if (!export_flag && !high_prec) {
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
	if (high_prec || export_flag) {
#if	CYGWIN
		fprintf(gfp, "\r\n");	/* might be redirecting to a Microsoft text file */
#else
		fprintf(gfp, "\n");
#endif
	} else {
		fprintf(gfp, "\n\n");
	}
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
	if (display2d) {
		/* display in fraction format */
		make_fractions_and_group(n);
		if (factor_int_flag) {
			factor_int_sub(n);
		}
		return flist_equation(n);
	} else {
		/* display in single-line format */
		if (factor_int_flag) {
			factor_int_sub(n);
		}
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
 * Convert the passed Mathomatic variable to an ASCII variable name.
 * The ASCII variable name is stored in the global var_str[].
 *
 * Return length of variable name (number of ASCII characters).
 *
 * If (lang_code == 0), use standard Mathomatic format.
 * If (lang_code == 1), make variable name C language compatible.
 * If (lang_code == 2), make variable name Java compatible.
 * If (lang_code == 3), make variable name Python compatible.
 * If lang_code is negative, create an exportable variable name: -1 for Maxima, -2 for other.
 */
int
list_var(v, lang_code)
long	v;		/* variable to convert */
int	lang_code;	/* language code */
{
	int		j;
	char		*cp, buf[100];

	cp = NULL;
	switch (v & VAR_MASK) {
	case V_NULL:
		cp = "null";
		break;
	case SIGN:
		cp = "sign";
		break;
	case IMAGINARY:
		switch (lang_code) {
		case -2:
			cp = "i";
			break;
		case -1:
			cp = "%i";
			break;
		case 0:
			cp = "i#";
			break;
		case 3:
			cp = "1j";
			break;
		default:
			cp = "I";
			break;
		}
		break;
	case V_E:
		switch (lang_code) {
		case -1:
			cp = "%e";
			break;
		case 0:
			cp = "e#";
			break;
		case 1:
			cp ="M_E";
			break;
		case 2:
			cp = "Math.E";
			break;
		case 3:
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
			cp = "pi#";
			break;
		case 1:
			cp = "M_PI";
			break;
		case 2:
			cp = "Math.PI";
			break;
		case 3:
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
		j = (v & VAR_MASK) - VAR_OFFSET;
		if (j >= 0) {
			cp = var_names[j];
		}
		break;
	}
	if (cp) {
		my_strlcpy(var_str, cp, sizeof(var_str));
	} else {
		my_strlcpy(var_str, "bad_variable", sizeof(var_str));
	}
	j = (v >> VAR_SHIFT) & SUBSCRIPT_MASK;
	if (j) {
		snprintf(buf, sizeof(buf), "%d", j - 1);
		strcat(var_str, buf);
	}
	return(strlen(var_str));
}

/*
 * Display an expression in single-line format.
 * Use color if color mode is set.
 * The expression is not modified.
 *
 * Return number of characters output (excluding escape sequences).
 */
int
list_proc(p1, n, export_flag)
token_type	*p1;		/* expression pointer */
int		n;		/* length of expression */
int		export_flag;	/* flag for exportable format (usually false) */
				/* 1 for Maxima, 2 for other */
{
	int	i, j, k, i1;
	int	min1;
	int	cur_level;
	char	*cp;
	int	len = 0;

	cur_level = min1 = min_level(p1, n);
	for (i = 0; i < n; i++) {
		j = cur_level - p1[i].level;
		k = abs(j);
		for (i1 = 1; i1 <= k; i1++) {
			if (j > 0) {
				cur_level--;
				len += fprintf(gfp, ")");
				set_color(cur_level-min1);
			} else {
				cur_level++;
				set_color(cur_level-min1);
				len += fprintf(gfp, "(");
			}
		}
		switch (p1[i].kind) {
		case CONSTANT:
			if (p1[i].token.constant == 0.0) {
				p1[i].token.constant = 0.0; /* fix -0 */
			}
			if (high_prec || export_flag) {
				if (p1[i].token.constant < 0.0) {
					len += fprintf(gfp, "(%.20g)", p1[i].token.constant);
				} else {
					len += fprintf(gfp, "%.20g", p1[i].token.constant);
				}
			} else if (finance_option) {
				if (p1[i].token.constant < 0.0) {
					len += fprintf(gfp, "(%.*f)", finance_option, p1[i].token.constant);
				} else {
					len += fprintf(gfp, "%.*f", finance_option, p1[i].token.constant);
				}
			} else {
				len += fprintf(gfp, "%.*g", precision, p1[i].token.constant);
			}
			break;
		case VARIABLE:
			list_var(p1[i].token.variable, 0 - export_flag);
			len += fprintf(gfp, "%s", var_str);
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
				if (power_starstar) {
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
			len += fprintf(gfp, "%s", cp);
			break;
		}
	}
	for (j = cur_level - min1; j > 0;) {
		len += fprintf(gfp, ")");
		j--;
		set_color(j);
	}
	return len;
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
	len = list_string_sub(lhs[n], n_lhs[n], NULL, export_flag);
	if (n_rhs[n]) {
		len += strlen(EQUATE_STRING);
		len += list_string_sub(rhs[n], n_rhs[n], NULL, export_flag);
	}
	len += 2;
	cp = (char *) malloc(len);
	if (cp == NULL) {
		error(_("Out of memory (can't malloc(3))."));
		return NULL;
	}
	list_string_sub(lhs[n], n_lhs[n], cp, export_flag);
	if (n_rhs[n]) {
		strcat(cp, EQUATE_STRING);
		list_string_sub(rhs[n], n_rhs[n], &cp[strlen(cp)], export_flag);
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
	len = list_string_sub(p1, n, NULL, export_flag);
	len++;
	cp = (char *) malloc(len);
	if (cp == NULL) {
		error(_("Out of memory (can't malloc(3))."));
		return NULL;
	}
	list_string_sub(p1, n, cp, export_flag);
	return cp;
}

/*
 * Convert an expression to a text string and store in "string" if
 * "string" is not NULL.  "string" need not be initialized,
 * but must be long enough to contain the expression.
 *
 * Return length (number of characters).
 */
int
list_string_sub(p1, n, string, export_flag)
token_type	*p1;		/* expression pointer */
int		n;		/* length of expression */
char		*string;	/* buffer or NULL pointer */
int		export_flag;
{
	int	i, j, k, i1;
	int	min1;
	int	cur_level;
	char	*cp;
	int	len = 0;
	char	buf[500];

	if (string)
		string[0] = '\0';
	cur_level = min1 = min_level(p1, n);
	for (i = 0; i < n; i++) {
		j = cur_level - p1[i].level;
		k = abs(j);
		for (i1 = 1; i1 <= k; i1++) {
			if (j > 0) {
				cur_level--;
				APPEND(")");
			} else {
				cur_level++;
				APPEND("(");
			}
		}
		switch (p1[i].kind) {
		case CONSTANT:
			if (p1[i].token.constant == 0.0) {
				p1[i].token.constant = 0.0; /* fix -0 */
			}
			if (high_prec || export_flag) {
				if (p1[i].token.constant < 0.0) {
					snprintf(buf, sizeof(buf), "(%.20g)", p1[i].token.constant);
				} else {
					snprintf(buf, sizeof(buf), "%.20g", p1[i].token.constant);
				}
			} else if (finance_option) {
				if (p1[i].token.constant < 0.0) {
					snprintf(buf, sizeof(buf), "(%.*f)", finance_option, p1[i].token.constant);
				} else {
					snprintf(buf, sizeof(buf), "%.*f", finance_option, p1[i].token.constant);
				}
			} else {
				snprintf(buf, sizeof(buf), "%.*g", precision, p1[i].token.constant);
			}
			APPEND(buf);
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
				if (power_starstar) {
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
	for (j = cur_level - min1; j > 0; j--) {
		APPEND(")");
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
		if ((p1[i].kind == CONSTANT && fmod(p1[i].token.constant, 1.0))
		    || (p1[i].kind == VARIABLE && p1[i].token.variable <= IMAGINARY)) {
			return false;
		}
	}
	return true;
}

/*
 * Display an equation as C, Java, or Python code.
 *
 * Return length of output (number of characters).
 */
int
list_c_equation(en, language, int_flag)
int	en;		/* equation space number */
int	language;
int	int_flag;	/* integer arithmetic flag */
{
	int	len = 0;

	if (n_lhs[en] <= 0)
		return 0;
	len += list_code(lhs[en], &n_lhs[en], language, int_flag);
	if (n_rhs[en]) {
		len += fprintf(gfp, " = ");
		len += list_code(rhs[en], &n_rhs[en], language, int_flag);
	}
	if (language != 2) {
		len += fprintf(gfp, ";");
	}
	fprintf(gfp, "\n");
	return len;
}

/*
 * Output C, Java, or Python code for an expression.
 * Expression might be modified by this function.
 *
 * Return length of output (number of characters).
 */
int
list_code(equation, np, language, int_flag)
token_type	*equation;	/* equation side pointer */
int		*np;		/* pointer to length of equation side */
int		language;	/* if 0, generate C code; if 1, generate Java code; if 2, generate Python code */
int		int_flag;	/* integer arithmetic flag, should work with any language */
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
								if (int_flag || language > 1)
									break;
								if (language) {
									cp = "Math.pow";
								} else {
									cp = "pow";
								}
								len += fprintf(gfp, cp);
							}
							break;
						case FACTORIAL:
							cp = "fact";
							len += fprintf(gfp, cp);
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
/* note: here we will need to parenthesize negative numbers to make -2**x work the same with Python: */
			if (int_flag) {
				snprintf(buf, sizeof(buf), "%.0f", equation[i].token.constant);
			} else {
				snprintf(buf, sizeof(buf), "%#.*g", MAX_PRECISION, equation[i].token.constant);
				j = strlen(buf) - 1;
				for (; j >= 0; j--) {
					if (buf[j] == '0')
						continue;
					if (buf[j] == '.') {
						buf[j+2] = '\0';
					} else {
						break;
					}
				}
			}
			if (equation[i].token.constant < 0) {
				len += fprintf(gfp, "(%s)", buf);
			} else {
				len += fprintf(gfp, "%s", buf);
			}
			break;
		case VARIABLE:
			len += list_var(equation[i].token.variable, language + 1);
			fprintf(gfp, "%s", var_str);
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
				if (int_flag || language > 1) {
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
 * Return the total width of the output (that is, the required number of screen columns)
 * or zero on failure.
 */
int
flist_equation(n)
int	n;	/* equation space number */
{
	int	sind;
	char	buf[30];
	int	len, len2, len3, width;
	int	pos;
	int	high, low;
	int	max_line, min_line;
	int	max2_line, min2_line;

	if (n_lhs[n] <= 0)
		return 0;
	len = snprintf(buf, sizeof(buf), "#%d: ", n + 1);
	cur_line = 0;
	cur_pos = 0;
	sind = n_rhs[n];
	len += flist(lhs[n], n_lhs[n], false, 0, &max_line, &min_line);
	if (n_rhs[n]) {
		len += strlen(EQUATE_STRING);
make_smaller:
		len2 = flist(rhs[n], sind, false, 0, &high, &low);
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
		len3 = flist(&rhs[n][sind], n_rhs[n] - sind, false, 0, &max2_line, &min2_line);
	} else {
		len2 = 0;
		len3 = 0;
	}
	width = max(len + len2, len3);
	if (screen_columns && gfp == stdout && width >= screen_columns) {
		/* output too wide to fit screen, output in single-line format */
		return list1_sub(n, false);
	}
	fprintf(gfp, "\n");
	for (cur_line = max_line; cur_line >= min_line; cur_line--) {
		pos = cur_pos = 0;
		if (cur_line == 0) {
			cur_pos += fprintf(gfp, "%s", buf);
		}
		pos += strlen(buf);
		pos += flist(lhs[n], n_lhs[n], true, pos, &high, &low);
		if (n_rhs[n]) {
			if (cur_line == 0) {
				cur_pos += fprintf(gfp, "%s", EQUATE_STRING);
			}
			pos += strlen(EQUATE_STRING);
			pos += flist(rhs[n], sind, true, pos, &high, &low);
		}
		fprintf(gfp, "\n");
	}
	if (sind < n_rhs[n]) {
/* output second half of split equation that was too wide to display on the screen without splitting */
		fprintf(gfp, "\n");
		for (cur_line = max2_line; cur_line >= min2_line; cur_line--) {
			cur_pos = 0;
			flist(&rhs[n][sind], n_rhs[n] - sind, true, 0, &high, &low);
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
 * Return the width of the expression (that is, the required number of screen columns).
 */
static int
flist(p1, n, out_flag, pos, highp, lowp)
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
	int	oflag;
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
		if (i == stop_at) {
			j = cur_level - p1[div_loc].level;
			k = abs(j) - 1;
		} else {
			j = cur_level - p1[i].level;
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
				set_color(level-1);
				for (j = 0; j < len_div; j++) {
					if (html_flag && gfp == stdout) {
						fprintf(gfp, "&mdash;");
					} else {
						fprintf(gfp, "-");
					}
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
			if (finance_option) {
				if (p1[i].token.constant < 0.0) {
					len += snprintf(buf, sizeof(buf), "(%.*f)", finance_option, p1[i].token.constant);
				} else {
					len += snprintf(buf, sizeof(buf), "%.*f", finance_option, p1[i].token.constant);
				}
			} else {
				len += snprintf(buf, sizeof(buf), "%.*g", precision, p1[i].token.constant);
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
			/* len must be incremented here by the number of columns the display of the operator takes */
			switch (p1[i].token.operatr) {
			case PLUS:
				cp = " + ";
				len += 3;
				break;
			case MINUS:
				if (html_flag && gfp == stdout) {
					cp = " &minus; ";
				} else {
					cp = " - ";
				}
				len += 3;
				break;
			case TIMES:
				if (html_flag && gfp == stdout) {
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
