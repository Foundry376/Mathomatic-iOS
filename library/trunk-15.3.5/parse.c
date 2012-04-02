/*
 * Expression parsing routines for Mathomatic.
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

int	point_flag;		/* point to location of parse error if true */

/*
 * Convert all alphabetic characters in a string to lower case.
 */
void
str_tolower(cp)
char	*cp;
{
	if (cp) {
		for (; *cp; cp++) {
			if (isupper(*cp))
				*cp = tolower(*cp);
		}
	}
}

/*
 * Display an up arrow pointing to the error, if appropriate.
 * The up arrow is positioned under the input string,
 * followed by the error message.
 */
void
put_up_arrow(cnt, cp)
int	cnt;	/* position of error, relative to "input_column" */
char	*cp;	/* error message */
{
#if	!SILENT
	int	 i;

	cnt += input_column;
	if (!quiet_mode && point_flag && (screen_columns == 0 || cnt < screen_columns)) {
		for (i = 0; i < cnt; i++) {
			printf(" ");
		}
		printf("^ ");
	}
#endif
	error(cp);
}

/*
 * Return true if character is a valid starting variable character.
 */
int
isvarchar(ch)
int	ch;
{
	return(ch == '_' || (ch && strchr(special_variable_characters, ch)) || isalpha(ch));
}

/*
 * Parenthesize an operator.
 */
void
binary_parenthesize(p1, n, i)
token_type	*p1;	/* pointer to expression */
int		n;	/* length of expression */
int		i;	/* location of operator to parenthesize in expression */
{
	int	j;
	int	level;

	if (i >= (n - 1) || (n & 1) != 1 || (i & 1) != 1) {
		error_bug("Internal error in arguments to binary_parenthesize().");
	}
	level = p1[i].level++;
	if (p1[i-1].level++ > level) {
		for (j = i - 2; j >= 0; j--) {
			if (p1[j].level <= level)
				break;
			p1[j].level++;
		}
	}
	if (p1[i+1].level++ > level) {
		for (j = i + 2; j < n; j++) {
			if (p1[j].level <= level)
				break;
			p1[j].level++;
		}
	}
}

/*
 * Handle and remove the special high priority NEGATE operator.
 * To make negate the same priority as times/divide, don't parenthesize here.
 */
void
handle_negate(equation, np)
token_type	*equation;
int		*np;
{
	int	i;

	for (i = 1; i < *np; i += 2) {
		if (equation[i].token.operatr == NEGATE) {
			/* change negate operator to -1 times the operand: */
			equation[i].token.operatr = TIMES;
			/* make negate top priority by parenthesizing it first: */
			binary_parenthesize(equation, *np, i);
		}
	}
}

/*
 * Parenthesize most operators so that expression evaluation is in the correct order.
 * Gives different operators on the same level in an expression the correct priority.
 * Similar operators on the same level are always evaluated or grouped left to right,
 * except for the power operator.
 *
 * organize() should be called after this to remove unneeded parentheses.
 */
void
give_priority(equation, np)
token_type	*equation;	/* pointer to expression */
int		*np;		/* pointer to expression length */
{
	int	i;

/* Higher priority (precedence) operators need to be parenthesized first: */
	for (i = 1; i < *np; i += 2) {
		if (equation[i].token.operatr == FACTORIAL) {
			binary_parenthesize(equation, *np, i);
		}
	}

	if (right_associative_power) {
		for (i = *np - 2; i >= 1; i -= 2) {	/* count down (for right to left evaluation) */
			if (equation[i].token.operatr == POWER) {
				binary_parenthesize(equation, *np, i);
			}
		}
	} else {
		for (i = 1; i < *np; i += 2) {		/* count up (for left to right evaluation) */
			if (equation[i].token.operatr == POWER) {
				binary_parenthesize(equation, *np, i);
			}
		}
	}

	for (i = 1; i < *np; i += 2) {
		switch (equation[i].token.operatr) {
		case TIMES:
		case DIVIDE:
		case MODULUS:
		case IDIVIDE:
			binary_parenthesize(equation, *np, i);
			break;
		}
	}
}

/*
 * This is a simple, non-recursive mathematical expression parser.
 * To parse, the character string is sequentially read and stored
 * in the internal storage format.
 * The maximum length that can be parsed is the size of an equation side (n_tokens).
 * Any syntax and other errors are carefully reported and give a NULL return.
 *
 * Returns the new string position, or NULL if error.
 */
char *
parse_section(equation, np, cp, allow_space)
token_type	*equation;	/* where the parsed expression is stored (equation side) */
int		*np;		/* pointer to the returned parsed expression length */
char		*cp;		/* string to parse */
int		allow_space;	/* if false, any space characters terminate parsing */
{
	int		n = 0;			/* position in equation[] */
	int		cur_level = 1;		/* current level of parentheses */
	int		operand = false;	/* flip-flop between operand and operator */
	char		*cp_start, *cp1;
	double		d;
	int		abs_count = 0;
	int		abs_array[10];

	if (cp == NULL)
		return(NULL);
	cp_start = cp;
	for (;; cp++) {
		if (n > (n_tokens - 10)) {
			error_huge();
		}
		switch (*cp) {
		case '(':
		case '{':
			if (operand) {
#if	1
/* Allow things like (x)(y) and x{y} by defaulting to the times operator.  The result is x*y. */
				operand = false;
				equation[n].level = cur_level;
				equation[n].kind = OPERATOR;
				equation[n].token.operatr = TIMES;
				n++;
#else
				goto syntax_error;
#endif
			}
			cur_level++;
			continue;
		case ')':
		case '}':
			cur_level--;
			if (cur_level <= 0 || (abs_count > 0 && cur_level < abs_array[abs_count-1])) {
				put_up_arrow(cp - cp_start, _("Unmatched parenthesis: too many )"));
				return(NULL);
			}
			if (!operand) {
				goto syntax_error;
			}
			continue;
		case ' ':
		case '\t':
			if (allow_space)
				continue;
		case '=':
		case ';':
		case '\0':
		case '\n':
			goto p_out;	/* terminator encountered */
		}
		operand = !operand;
		switch (*cp) {
		case '|':
			if (operand) {
				if (abs_count >= ARR_CNT(abs_array)) {
					error(_("Too many nested absolute values."));
					return(NULL);
				}
				cur_level += 3;
				abs_array[abs_count++] = cur_level;
			} else {
				if (abs_count <= 0 || cur_level != abs_array[--abs_count]) {
					goto syntax_error;
				}
				cur_level--;
				equation[n].level = cur_level;
				equation[n].kind = OPERATOR;
				equation[n].token.operatr = POWER;
				n++;
				equation[n].level = cur_level;
				equation[n].kind = CONSTANT;
				equation[n].token.constant = 2.0;
				n++;
				cur_level--;
				equation[n].level = cur_level;
				equation[n].kind = OPERATOR;
				equation[n].token.operatr = POWER;
				n++;
				equation[n].level = cur_level;
				equation[n].kind = CONSTANT;
				equation[n].token.constant = 0.5;
				n++;
				cur_level--;
			}
			operand = !operand;
			break;
		case '!':
			if (operand) {
				goto syntax_error;
			}
			if (cp[1] == '!' && cp[2] != '!') {
				warning(_("Multifactorial not implemented, using x!! = (x!)!"));
			}
			equation[n].level = cur_level;
			equation[n].kind = OPERATOR;
			equation[n].token.operatr = FACTORIAL;
			n++;
			equation[n].level = cur_level;
			equation[n].kind = CONSTANT;
			equation[n].token.constant = 1.0;
			n++;
			operand = true;
			break;
		case '^':
parse_power:
			if (operand) {
				goto syntax_error;
			}
			equation[n].level = cur_level;
			equation[n].kind = OPERATOR;
			equation[n].token.operatr = POWER;
			n++;
			break;
		case '*':
			if (cp[1] == '*') {	/* for "**" */
				cp++;
				goto parse_power;
			}
			if (operand) {
				goto syntax_error;
			}
			equation[n].level = cur_level;
			equation[n].kind = OPERATOR;
			equation[n].token.operatr = TIMES;
			n++;
			break;
		case '/':
			if (operand) {
				goto syntax_error;
			}
			if (cp[1] == '/') {
				cp++;
				equation[n].level = cur_level;
				equation[n].kind = OPERATOR;
				equation[n].token.operatr = IDIVIDE;
			} else {
				equation[n].level = cur_level;
				equation[n].kind = OPERATOR;
				equation[n].token.operatr = DIVIDE;
			}
			n++;
			break;
		case '%':
			if (operand) {
#if	1
/* Allow %e, %i, and %pi by ignoring % in the wrong place. */
				operand = false;
				break;
#else
				goto syntax_error;
#endif
			}
			equation[n].level = cur_level;
			equation[n].kind = OPERATOR;
			equation[n].token.operatr = MODULUS;
			n++;
			break;
		case '+':
		case '-':
			if (!operand) {
				equation[n].level = cur_level;
				equation[n].kind = OPERATOR;
				equation[n].token.operatr = ((*cp == '+') ? PLUS : MINUS);
				n++;
			}
			if (strncmp(cp, "+/-", 3) == 0) {
				equation[n].level = cur_level;
				equation[n].kind = VARIABLE;
				next_sign(&equation[n].token.variable);
				n++;
				equation[n].level = cur_level;
				equation[n].kind = OPERATOR;
				equation[n].token.operatr = TIMES;
				n++;
				cp += 2;
				operand = false;
				break;
			}
			if (!operand) {
				break;
			}
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '.':
			if (!operand) {
				operand = true;
				equation[n].level = cur_level;
				equation[n].kind = OPERATOR;
				equation[n].token.operatr = TIMES;
				n++;
			}
			if (*cp == '-') {
				equation[n].kind = CONSTANT;
				equation[n].token.constant = -1.0;
				equation[n].level = cur_level;
				n++;
				equation[n].kind = OPERATOR;
				equation[n].token.operatr = NEGATE;
				equation[n].level = cur_level;
				n++;
				operand = false;
				continue;
			}
			cp1 = cp;
			errno = 0;
			d = strtod(cp1, &cp);
			if (cp == cp1) {
				goto syntax_error;
			}
			if (errno) {
				put_up_arrow(cp1 - cp_start, _("Constant out of range."));
				return(NULL);
			}
			equation[n].kind = CONSTANT;
			equation[n].token.constant = d;
			equation[n].level = cur_level;
			n++;
			cp--;
			break;
		default:
			if (!isvarchar(*cp)) {
				put_up_arrow(cp - cp_start, _("Unrecognized character."));
				return(NULL);
			}
			if (!operand) {
				operand = true;
				equation[n].level = cur_level;
				equation[n].kind = OPERATOR;
				equation[n].token.operatr = TIMES;
				n++;
			}
			cp1 = cp;
			if (strncasecmp(cp, INFINITY_NAME, strlen(INFINITY_NAME)) == 0
			    && !isvarchar(cp[strlen(INFINITY_NAME)])) {
				equation[n].kind = CONSTANT;
				equation[n].token.constant = INFINITY;	/* the infinity constant */
				cp += strlen(INFINITY_NAME);
			} else {
				equation[n].kind = VARIABLE;
				cp = parse_var(&equation[n].token.variable, cp);
				if (cp == NULL) {
					return(NULL);
				}
			}
			if (*cp == '(') {
				put_up_arrow(cp1 - cp_start, _("Named functions currently not implemented, except when using m4."));
				return(NULL);
			}
			cp--;
			equation[n].level = cur_level;
			n++;
			break;
		}
	}
p_out:
	if (abs_count != 0 || (n && !operand)) {
		goto syntax_error;
	}
	if (cur_level != 1) {
		put_up_arrow(cp - cp_start, _("Unmatched parenthesis: missing )"));
		return(NULL);
	}
	while (*cp == '=')
		cp++;
	*np = n;
	if (n) {
		handle_negate(equation, np);
		give_priority(equation, np);
		organize(equation, np);
	}
	input_column += (cp - cp_start);
	return cp;

syntax_error:
	put_up_arrow(cp - cp_start, _("Syntax error."));
	return(NULL);
}

/*
 * Parse an equation string into equation space "n".
 * The result will not necessarily be an equation,
 * because this can parse any expression or equation.
 *
 * Returns the new string position, or NULL if error.
 * Currently, there can be no more to parse in the string when this returns.
 */
char *
parse_equation(n, cp)
int	n;	/* equation space number */
char	*cp;	/* pointer to the beginning of the equation character string */
{
	if (cp == NULL)
		return NULL;
	if (!case_sensitive_flag) {
		str_tolower(cp);
	}
	if ((cp = parse_section(lhs[n], &n_lhs[n], cp, true)) != NULL) {
		if ((cp = parse_section(rhs[n], &n_rhs[n], cp, true)) != NULL) {
			if (!extra_characters(cp))
				return cp;
		}
	}
	n_lhs[n] = 0;
	n_rhs[n] = 0;
	return NULL;
}

/*
 * Parse an expression (not an equation) string, with equation space pull
 * if the string starts with "#" followed by an equation number.
 *
 * Returns the new string position, or NULL if error.
 * Currently, there can be no more to parse in the string when this returns.
 */
char *
parse_expr(equation, np, cp)
token_type	*equation;	/* where the parsed expression is stored (equation side) */
int		*np;		/* pointer to the returned parsed expression length */
char		*cp;		/* string to parse */
{
	int	i;
	char	*cp1, *cp2;

	if (cp == NULL)
		return NULL;
	if (!case_sensitive_flag) {
		str_tolower(cp);
	}
	cp1 = skip_space(cp);
	if (*cp1 == '#') {
		cp1++;
		switch (*cp1) {
		case '+':
		case '-':
			i = decstrtol(cp1, &cp2);
			i = cur_equation + i;
			break;
		default:
			i = decstrtol(cp1, &cp2) - 1;
			break;
		}
		if (cp1 == cp2 || *cp2) {
			error(_("Error parsing equation space number."));
			return NULL;
		}
		if (i < 0 || i >= n_equations || n_lhs[i] <= 0) {
			error(_("No expression available in specified equation space."));
			return NULL;
		}
		if (n_rhs[i]) {
			blt(equation, rhs[i], n_rhs[i] * sizeof(token_type));
			*np = n_rhs[i];
		} else {
			blt(equation, lhs[i], n_lhs[i] * sizeof(token_type));
			*np = n_lhs[i];
		}
#if	!SILENT
		list_proc(equation, *np, false);
		fprintf(gfp, "\n");
#endif
		return cp2;
	}
	if ((cp = parse_section(equation, np, cp, true)) != NULL) {
		if (extra_characters(cp))
			return NULL;
	}
	return cp;
}

/*
 * Parse variable name string pointed to by "cp".
 * The variable name is converted to Mathomatic format and stored in "*vp".
 *
 * If the variable is not special and never existed before, it is created.
 *
 * Return new string position if successful.
 * Display error message and return NULL on failure.
 */
char *
parse_var(vp, cp)
long	*vp;
char	*cp;
{
	int	i, j;
	long	vtmp;
	char	buf[MAX_VAR_LEN+1];
	char	*cp1;
	int	len;
	int	(*strcmpfunc)();

/*	*vp = V_NULL; */
	if (case_sensitive_flag) {
		strcmpfunc = strcmp;
	} else {
		strcmpfunc = strcasecmp;
	}
	if (!isvarchar(*cp)) {
		error(_("Invalid variable."));
		return(NULL);	/* variable name must start with a valid variable character */
	}
	for (cp1 = cp, i = 0; *cp1;) {
		if (!isvarchar(*cp1)) {
			break;
		}
		if (i >= MAX_VAR_LEN) {
			error(_("Variable name too long."));
			return(NULL);
		}
		buf[i++] = *cp1++;
	}
	buf[i] = '\0';

	if (strcasecmp(buf, NAN_NAME) == 0) {
		warning(_("Attempt to enter NaN (Not a Number); Converted to variable."));
	}
	if (strcasecmp(buf, INFINITY_NAME) == 0) {
		error(_("Infinity cannot be used as a variable."));
		return(NULL);
	} else if ((*strcmpfunc)(buf, "sign") == 0) {
		vtmp = SIGN;
	} else {
		if (strncasecmp(cp, "i#", 2) == 0) {
			*vp = IMAGINARY;
			return(cp + 2);
		}
		if (strncasecmp(cp, "e#", 2) == 0) {
			*vp = V_E;
			return(cp + 2);
		}
		if (strncasecmp(cp, "pi#", 3) == 0) {
			*vp = V_PI;
			return(cp + 3);
		}
		for (cp1 = cp, i = 0; *cp1;) {
			if (!isvarchar(*cp1) && !isdigit(*cp1)) {
				break;
			}
			if (i >= MAX_VAR_LEN) {
				error(_("Variable name too long."));
				return(NULL);
			}
			buf[i++] = *cp1++;
		}
		buf[i] = '\0';
		if ((*strcmpfunc)(buf, "i") == 0) {
			*vp = IMAGINARY;
			return(cp1);
		}
		if ((*strcmpfunc)(buf, "e") == 0) {
			*vp = V_E;
			return(cp1);
		}
		if ((*strcmpfunc)(buf, "pi") == 0) {
			*vp = V_PI;
			return(cp1);
		}
		if (is_all(buf)) {
			error(_("\"all\" is a reserved word and may not be used as a variable name."));
			return(NULL);
		}
		vtmp = 0;
		for (i = 0; var_names[i]; i++) {
			if ((*strcmpfunc)(buf, var_names[i]) == 0) {
				vtmp = i + VAR_OFFSET;
				break;
			}
		}
		if (vtmp == 0) {
			if (i >= (MAX_VAR_NAMES - 1)) {
				error(_("Maximum number of variable names reached."));
#if	!SILENT
				printf(_("Please restart or use \"clear all\".\n"));
#endif
				return(NULL);
			}
			len = strlen(buf) + 1;
			var_names[i] = (char *) malloc(len);
			if (var_names[i] == NULL) {
				error(_("Out of memory (can't malloc(3) variable name)."));
				return(NULL);
			}
			blt(var_names[i], buf, len);
			vtmp = i + VAR_OFFSET;
			var_names[i+1] = NULL;
		}
		*vp = vtmp;
		return cp1;
	}
/* for "sign" variables: */
	if (isdigit(*cp1)) {
		j = strtol(cp1, &cp1, 10);
		if (j < 0 || j > MAX_SUBSCRIPT) {
			error(_("Maximum subscript exceeded in special variable name."));
			return(NULL);
		}
		if (vtmp == SIGN) {
			sign_array[j+1] = true;
		}
		vtmp += ((long) (j + 1)) << VAR_SHIFT;
	}
	*vp = vtmp;
	return cp1;
}

/*
 * Remove trailing spaces from a string.
 */
void
remove_trailing_spaces(cp)
char	*cp;
{
	int	i;

	i = strlen(cp) - 1;
	while (i >= 0 && isspace(cp[i])) {
		cp[i] = '\0';
		i--;
	}
}

/*
 * This should be called for all line input.
 * Set point_flag to true if pointing to the input error will work for the passed string.
 * Truncate string to the actual content.
 */
void
set_error_level(cp)
char	*cp;	/* input string */
{
	char	*cp1;
	int	len;

	point_flag = true;
/* handle comments, newlines, and the DOS EOF character (control-Z) by truncating the string where found */
	cp1 = cp;
	while ((cp1 = strpbrk(cp1, ";\n\r\032"))) {
		if (cp1 != cp && *(cp1 - 1) == '\\') {
			if (*cp1 == ';') {
/* skip backslash escaped semicolon */
				len = strlen(cp1);
				blt(cp1 - 1, cp1, len + 1);
				continue;
			}
		}
		break;
	}
	if (cp1) {
		*cp1 = '\0';
	}
	remove_trailing_spaces(cp);
/* set point_flag to false if non-printable characters encountered */
	for (cp1 = cp; *cp1; cp1++) {
		if (!isprint(*cp1)) {
			point_flag = false;
		}
	}
}

/*
 * Return true if passed variable "v" is a constant.
 * Return value of constant in "*dp".
 */
int
var_is_const(v, dp)
long	v;
double	*dp;
{
	switch (v) {
	case V_E:
		if (dp)
			*dp = M_E;
		return true;
	case V_PI:
		if (dp)
			*dp = M_PI;
		return true;
	}
	return false;
}

/*
 * Substitute E and PI variables with their respective constants.
 */
int
subst_constants(equation, np)
token_type	*equation;
int		*np;
{
	int	i;
	int	modified = false;
	double	d;

	for (i = 0; i < *np; i += 2) {
		if (equation[i].kind == VARIABLE) {
			if (var_is_const(equation[i].token.variable, &d)) {
				equation[i].kind = CONSTANT;
				equation[i].token.constant = d;
				modified = true;
			}
		}
	}
	return modified;
}

#ifndef	my_strlcpy
/*
 * A very efficient strlcpy().
 *
 * Copy up to (n - 1) characters from string in src
 * to dest and null-terminate the result.
 *
 * Return length of src.
 */
int
my_strlcpy(dest, src, n)
char	*dest, *src;
int	n;
{
	int	len, len_src;

	len_src = strlen(src);
	len = min(n - 1, len_src);
	memmove(dest, src, len);
	dest[len] = '\0';
	return len_src;
}
#endif
