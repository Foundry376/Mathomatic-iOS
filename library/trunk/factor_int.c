/*
 * Mathomatic floating point constant factorizing routines.
 *
 * Copyright (C) 1987-2009 George Gesslein II.
 */

#include "includes.h"

static void	try_factor();
static int	fc_recurse();

/* The following data is used to factor integers: */
static double nn, vv;
static double skip_multiples[] = {	/* Additive array that skips over multiples of 2, 3, 5, and 7. */
	10, 2, 4, 2, 4, 6, 2, 6,
	 4, 2, 4, 6, 6, 2, 6, 4,
	 2, 6, 4, 6, 8, 4, 2, 4,
	 2, 4, 8, 6, 4, 6, 2, 4,
	 6, 2, 6, 6, 4, 2, 4, 6,
	 2, 6, 4, 2, 4, 2,10, 2
};	/* sum of all numbers = 210 = (2*3*5*7) */

/*
 * Factor the integer in "value".
 * Store the prime factors in the unique[] array.
 *
 * Return true if successful.
 */
int
factor_one(value)
double	value;
{
	int	i;
	double	d;

	uno = 0;
	nn = value;
	if (nn == 0.0) {
		return false;
	}
	if (fabs(nn) >= MAX_K_INTEGER) {
		/* too large to factor */
		return false;
	}
	if (fmod(nn, 1.0) != 0.0) {
		/* not an integer */
		return false;
	}
	vv = 1.0 + sqrt(fabs(nn));
	try_factor(2.0);
	try_factor(3.0);
	try_factor(5.0);
	try_factor(7.0);
	d = 1.0;
	while (d <= vv) {
		for (i = 0; i < ARR_CNT(skip_multiples); i++) {
			d += skip_multiples[i];
			try_factor(d);
		}
	}
	if (nn != 1.0) {
		try_factor(nn);
	}
	if (value != multiply_out_unique()) {
		error_bug("Internal error factoring integers.");
	}
	return true;
}

/*
 * See if "arg" is one or more factors of "nn".
 * If so, save it and remove it from "nn".
 */
static void
try_factor(arg)
double	arg;
{
	while (fmod(nn, arg) == 0.0) {
		if (uno > 0 && unique[uno-1] == arg) {
			ucnt[uno-1]++;
		} else {
			unique[uno] = arg;
			ucnt[uno] = 1;
			uno++;
		}
		nn /= arg;
		vv = 1.0 + sqrt(fabs(nn));
		if (nn <= 1.0 && nn >= -1.0)
			break;
	}
}

/*
 * Convert unique[] back into the single integer it represents,
 * which was the value passed in the last call to factor_one(value).
 * Nothing is changed and the value is returned.
 */
double
multiply_out_unique()
{
	int	i, j;
	double	d;

	d = 1.0;
	for (i = 0; i < uno; i++) {
		for (j = 0; j < ucnt[i]; j++) {
			d *= unique[i];
		}
	}
	return d;
}

/*
 * Display the integer prime factors in the unique[] array.
 * Must have had a successful call to factor_one(value) previously,
 * to fill out unique[] with the prime factors of value.
 */
void
display_unique()
{
	int	i;
	double	value;

	value = multiply_out_unique();
	fprintf(gfp, "%.0f = ", value);
	for (i = 0; i < uno;) {
		fprintf(gfp, "%.0f", unique[i]);
		if (ucnt[i] > 1) {
			fprintf(gfp, "^%d", ucnt[i]);
		}
		i++;
		if (i < uno) {
			fprintf(gfp, " * ");
		}
	}
	fprintf(gfp, "\n");
}

/*
 * Determine if the result of factor_one(x) is prime.
 *
 * Return true if x is a prime number.
 */
int
is_prime()
{
	double	value;

	value = multiply_out_unique();
	if (value < 2.0)
		return false;
	if (uno == 1 && ucnt[0] == 1)
		return true;
	return false;
}

/*
 * Factor integers in an equation side.
 *
 * Return true if equation side was modified.
 */
int
factor_int(equation, np)
token_type	*equation;
int		*np;
{
	int	i, j;
	int	xsize;
	int	level;
	int	modified = false;

	for (i = 0; i < *np; i += 2) {
		if (equation[i].kind == CONSTANT && factor_one(equation[i].token.constant) && uno > 0) {
			if (uno == 1 && ucnt[0] <= 1)
				continue;	/* prime number */
			level = equation[i].level;
			if (uno > 1 && *np > 1)
				level++;
			xsize = -2;
			for (j = 0; j < uno; j++) {
				if (ucnt[j] > 1)
					xsize += 4;
				else
					xsize += 2;
			}
			if (*np + xsize > n_tokens) {
				error_huge();
			}
			for (j = 0; j < uno; j++) {
				if (ucnt[j] > 1)
					xsize = 4;
				else
					xsize = 2;
				if (j == 0)
					xsize -= 2;
				if (xsize > 0) {
					blt(&equation[i+xsize], &equation[i], (*np - i) * sizeof(token_type));
					*np += xsize;
					if (j > 0) {
						i++;
						equation[i].kind = OPERATOR;
						equation[i].level = level;
						equation[i].token.operatr = TIMES;
						i++;
					}
				}
				equation[i].kind = CONSTANT;
				equation[i].level = level;
				equation[i].token.constant = unique[j];
				if (ucnt[j] > 1) {
					equation[i].level = level + 1;
					i++;
					equation[i].kind = OPERATOR;
					equation[i].level = level + 1;
					equation[i].token.operatr = POWER;
					i++;
					equation[i].level = level + 1;
					equation[i].kind = CONSTANT;
					equation[i].token.constant = ucnt[j];
				}
			}
			modified = true;
		}
	}
	return modified;
}

/*
 * Factor integers in an equation space.
 */
void
factor_int_sub(n)
int	n;	/* equation space number */
{
	if (n_lhs[n] <= 0)
		return;
	factor_int(lhs[n], &n_lhs[n]);
	factor_int(rhs[n], &n_rhs[n]);
}

/*
 * List an equation side with optional integer factoring.
 */
int
list_factor(equation, np, factor_flag)
token_type	*equation;
int		*np;
int		factor_flag;
{
	if (factor_flag || factor_int_flag) {
		factor_int(equation, np);
	}
	return list_proc(equation, *np, false);
}

/*
 * Neatly factor out coefficients in additive expressions in an equation side.
 * For example: (2*x + 4*y + 6) becomes 2*(x + 2*y + 3).
 *
 * This routine is often necessary because the expression compare (se_compare())
 * does not return a multiplier (except for +/-1.0).
 * Normalization done here is required for simplification of algebraic fractions, etc.
 *
 * If "level_code" is 0, all additive expressions are normalized
 * by making at least one coefficient unity (1) by factoring out
 * the absolute value of the constant coefficient closest to zero.
 * This makes the absolute value of all other coefficients >= 1.
 * If all coefficients are negative, -1 will be factored out, too.
 *
 * If "level_code" is 1, any level 1 additive expression is factored
 * nicely for readability, while all deeper levels are normalized,
 * so that algebraic fractions are simplified.
 *
 * If "level_code" is 2, nothing is normalized unless it increases
 * readability.
 *
 * If "level_code" is 3, nothing is done.
 *
 * Return true if equation side was modified.
 */
int
factor_constants(equation, np, level_code)
token_type	*equation;	/* pointer to the beginning of equation side */
int		*np;		/* pointer to length of equation side */
int		level_code;	/* see above */
{
	if (level_code > 2)
		return false;
	return fc_recurse(equation, np, 0, 1, level_code);
}

static int
fc_recurse(equation, np, loc, level, level_code)
token_type	*equation;
int		*np, loc, level;
int		level_code;
{
	int	i, j, k;
	int	op;
	double	d, minimum = 1.0, cogcd = 1.0;
	int	improve_readability, first = true, neg_flag = true, modified = false;
	int	op_count = 0, const_count = 0;

	for (i = loc; i < *np && equation[i].level >= level;) {
		if (equation[i].level > level) {
			modified |= fc_recurse(equation, np, i, level + 1, level_code);
			i++;
			for (; i < *np && equation[i].level > level; i += 2)
				;
			continue;
		}
		i++;
	}
	if (modified)
		return true;
	improve_readability = (level_code > 1 || (level_code && (level == 1)));
	for (i = loc; i < *np && equation[i].level >= level;) {
		if (equation[i].level == level) {
			switch (equation[i].kind) {
			case CONSTANT:
				const_count++;
				d = equation[i].token.constant;
				break;
			case OPERATOR:
				switch (equation[i].token.operatr) {
				case PLUS:
					neg_flag = false;
				case MINUS:
					op_count++;
					break;
				default:
					return modified;
				}
				i++;
				continue;
			default:
				d = 1.0;
				break;
			}
			if (i == loc && d > 0.0)
				neg_flag = false;
			d = fabs(d);
			if (first) {
				minimum = d;
				cogcd = d;
				first = false;
			} else {
				if (minimum > d)
					minimum = d;
				if (integer_coefficients && improve_readability && cogcd)
					cogcd = gcd_verified(d, cogcd);
			}
		} else {
			op = 0;
			for (j = i + 1; j < *np && equation[j].level > level; j += 2) {
				if (equation[j].level == level + 1) {
					op = equation[j].token.operatr;
				}
			}
			if (op == TIMES || op == DIVIDE) {
				for (k = i; k < j; k++) {
					if (equation[k].level == (level + 1) && equation[k].kind == CONSTANT) {
						if (i == j)
							return modified; /* more than one constant */
						if (k > i && equation[k-1].token.operatr != TIMES)
							return modified;
						d = equation[k].token.constant;
						if (i == loc && d > 0.0)
							neg_flag = false;
						d = fabs(d);
						if (first) {
							minimum = d;
							cogcd = d;
							first = false;
						} else {
							if (minimum > d)
								minimum = d;
							if (integer_coefficients && improve_readability && cogcd)
								cogcd = gcd_verified(d, cogcd);
						}
						i = j;
					}
				}
				if (i == j)
					continue;
			}
			if (i == loc)
				neg_flag = false;
			if (first) {
				minimum = 1.0;
				cogcd = 1.0;
				first = false;
			} else {
				if (minimum > 1.0)
					minimum = 1.0;
				if (integer_coefficients && improve_readability && cogcd)
					cogcd = gcd_verified(1.0, cogcd);
			}
			i = j;
			continue;
		}
		i++;
	}
	if (integer_coefficients && improve_readability && cogcd && fmod(cogcd, 1.0) == 0.0) {
		minimum = cogcd;
	}
	if (first || op_count == 0 || const_count > 1 || (!neg_flag && minimum == 1.0))
		return modified;
	if (minimum == 0.0 || !isfinite(minimum))
		return modified;
	if (improve_readability) {
		for (i = loc;;) {
			d = 1.0;
			if (equation[i].kind == CONSTANT) {
				if (equation[i].level == level || ((i + 1) < *np
				    && equation[i].level == (level + 1)
				    && equation[i+1].level == (level + 1)
				    && (equation[i+1].token.operatr == TIMES
				    || equation[i+1].token.operatr == DIVIDE))) {
					d = equation[i].token.constant;
				}
			}
			if ((minimum < 1.0 && fmod(d, 1.0) == 0.0) || (fmod(d, minimum) != 0.0)) {
				minimum = 1.0;
				break;
			}
			i++;
			for (; i < *np && equation[i].level > level; i += 2)
				;
			if (i >= *np || equation[i].level < level)
				break;
			i++;
		}
	}
	if (neg_flag)
		minimum = -minimum;
	if (minimum == 1.0)
		return modified;
	if (*np + ((op_count + 2) * 2) > n_tokens) {
		error_huge();
	}
	for (i = loc; i < *np && equation[i].level >= level; i++) {
		if (equation[i].kind != OPERATOR) {
			for (j = i;;) {
				equation[j].level++;
				j++;
				if (j >= *np || equation[j].level <= level)
					break;
			}
			blt(&equation[j+2], &equation[j], (*np - j) * sizeof(token_type));
			*np += 2;
			equation[j].level = level + 1;
			equation[j].kind = OPERATOR;
			equation[j].token.operatr = DIVIDE;
			j++;
			equation[j].level = level + 1;
			equation[j].kind = CONSTANT;
			equation[j].token.constant = minimum;
			i = j;
		}
	}
	for (i = loc; i < *np && equation[i].level >= level; i++) {
		equation[i].level++;
	}
	blt(&equation[i+2], &equation[i], (*np - i) * sizeof(token_type));
	*np += 2;
	equation[i].level = level;
	equation[i].kind = OPERATOR;
	equation[i].token.operatr = TIMES;
	i++;
	equation[i].level = level;
	equation[i].kind = CONSTANT;
	equation[i].token.constant = minimum;
	return true;
}
