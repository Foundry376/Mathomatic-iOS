/*
 * Floating point complex number routines specifically for Mathomatic.
 *
 * Copyright (C) 1987-2008 George Gesslein II.
 */

#include "includes.h"

/*
 * Convert doubles x and y from rectangular coordinates to polar coordinates.
 *
 * The amplitude is stored in *radiusp and the angle in radians is stored in *thetap.
 */
void
rect_to_polar(x, y, radiusp, thetap)
double	x, y, *radiusp, *thetap;
{
	*radiusp = sqrt(x * x + y * y);
	*thetap = atan2(y, x);
}

#if	!LIBRARY
/*
 * The roots command.
 */
int
roots_cmd(cp)
char	*cp;
{
#define	MAX_ROOT	10000.0	/* root limit needed because more roots become more inaccurate and take longer to calculate */

	complexs	c, c2, check;
	double		d, k;
	double		root;
	double		radius, theta;
	double		radius_root = 0.0;
	char		buf[MAX_CMD_LEN];

	if (*cp == '\0') {
		my_strlcpy(prompt_str, _("Enter root (positive integer): "), sizeof(prompt_str));
		if ((cp = get_string(buf, sizeof(buf))) == NULL)
			return false;
	}
	root = strtod(cp, &cp);
	if ((*cp && !isspace(*cp)) || root < 0.0 || root > MAX_ROOT || fmod(root, 1.0) != 0.0) {
		printf(_("Root must be a positive integer less than or equal to %.0f.\n"), MAX_ROOT);
		return false;
	}
	cp = skip_space(cp);
	if (*cp == '\0') {
		my_strlcpy(prompt_str, _("Enter real part (X): "), sizeof(prompt_str));
		if ((cp = get_string(buf, sizeof(buf))) == NULL)
			return false;
	}
	c.re = strtod(cp, &cp);
	if (*cp && !isspace(*cp)) {
		printf(_("Invalid number.\n"));
		return false;
	}
	cp = skip_space(cp);
	if (*cp == '\0') {
		my_strlcpy(prompt_str, _("Enter imaginary part (Y): "), sizeof(prompt_str));
		if ((cp = get_string(buf, sizeof(buf))) == NULL)
			return false;
	}
	c.im = strtod(cp, &cp);
	if (*cp) {
		printf(_("Invalid number.\n"));
		return false;
	}
	if (c.re == 0.0 && c.im == 0.0) {
		return false;
	}
/* convert to polar coordinates */
	errno = 0;
	rect_to_polar(c.re, c.im, &radius, &theta);
	if (root) {
		radius_root = pow(radius, 1.0 / root);
	}
	check_err();
	fprintf(gfp, _("\nThe polar coordinates are:\n%.*g amplitude and %.*g radians (%.*g degrees).\n\n"),
	    precision, radius, precision, theta, precision, theta * 180.0 / M_PI);
	if (root) {
		if (c.im == 0.0) {
			fprintf(gfp, _("The %.12g roots of (%.12g)^(1/%.12g) are:\n\n"), root, c.re, root);
		} else {
			fprintf(gfp, _("The %.12g roots of (%.12g%+.12g*i)^(1/%.12g) are:\n\n"), root, c.re, c.im, root);
		}
		for (k = 0.0; k < root; k += 1.0) {
/* add constants to theta and convert back to rectangular coordinates */
			c2.re = radius_root * cos((theta + 2.0 * k * M_PI) / root);
			c2.im = radius_root * sin((theta + 2.0 * k * M_PI) / root);
			complex_fixup(&c2);
			if (c2.im == 0.0) {
				fprintf(gfp, "%.12g\n", c2.re);
			} else {
				fprintf(gfp, "%.12g %+.12g*i\n", c2.re, c2.im);
			}
			check = c2;
			for (d = 1.0; d < root; d += 1.0) {
				check = complex_mult(check, c2);
			}
			complex_fixup(&check);
			if (check.im == 0.0) {
				printf(_("Inverse check: %.12g\n\n"), check.re);
			} else {
				printf(_("Inverse check: %.12g %+.12g*i\n\n"), check.re, check.im);
			}
		}
	}
	return true;
}
#endif

/*
 * Approximate roots of complex numbers in an equation side:
 * (complex^real) and (real^complex) and (complex^complex).
 * This only gives one root, even when there may be many roots.
 *
 * Return true and display a warning message if the equation side was modified.
 */
int
complex_root_simp(equation, np)
token_type	*equation;	/* equation side pointer */
int		*np;		/* pointer to length of equation side */
{
	int		i, j;
	int		level;
	int		len;
	complexs	c, p;
	int		modified = false;

start_over:
	for (i = 1; i < *np; i += 2) {
		if (equation[i].token.operatr != POWER)
			continue;
		level = equation[i].level;
		for (j = i + 2; j < *np && equation[j].level >= level; j += 2)
			;
		len = j - (i + 1);
		if (!parse_complex(&equation[i+1], len, &p))
			continue;
		for (j = i - 1; j >= 0 && equation[j].level >= level; j--)
			;
		j++;
		if (!parse_complex(&equation[j], i - j, &c))
			continue;
		if (c.im == 0.0 && p.im == 0.0)
			continue;
		i += len + 1;
		c = complex_pow(c, p);
		if (*np + 5 - (i - j) > n_tokens) {
			error_huge();
		}
		blt(&equation[j+5], &equation[i], (*np - i) * sizeof(token_type));
		*np += 5 - (i - j);
		equation[j].level = level;
		equation[j].kind = CONSTANT;
		equation[j].token.constant = c.re;
		j++;
		equation[j].level = level;
		equation[j].kind = OPERATOR;
		equation[j].token.operatr = PLUS;
		j++;
		level++;
		equation[j].level = level;
		equation[j].kind = CONSTANT;
		equation[j].token.constant = c.im;
		j++;
		equation[j].level = level;
		equation[j].kind = OPERATOR;
		equation[j].token.operatr = TIMES;
		j++;
		equation[j].level = level;
		equation[j].kind = VARIABLE;
		equation[j].token.variable = IMAGINARY;
		modified = true;
		goto start_over;
	}
	if (modified) {
		warning(_("Complex number root approximated."));
	}
	return modified;
}

/*
 * Get a constant, if the passed expression evaluates to a constant.
 * This should not be called from low level routines.
 *
 * Return true if successful, with the floating point constant returned in *dp.
 */
int
get_constant(p1, n, dp)
token_type	*p1;	/* expression pointer */
int		n;	/* length of expression */
double		*dp;	/* pointer to returned double */
{
	int	i, j;
	int	level;
	double	d1, d2;
	int	prev_approx_flag;

	if (n == 1) {
		switch (p1[0].kind) {
		case CONSTANT:
			*dp = p1[0].token.constant;
			return true;
		case VARIABLE:
			if (var_is_const(p1[0].token.variable, dp)) {
				return true;
			}
			break;
		case OPERATOR:
			break;
		}
	} else if (n >= 3) {
		level = p1[1].level;
		if (!get_constant(p1, 1, &d1))
			return false;
		for (i = 1; i < n; i = j) {
			if (p1[i].kind != OPERATOR || p1[i].level > level)
				return false;
			level = p1[i].level;
			for (j = i + 2; j < n && p1[j].level > level; j += 2)
				;
			if (!get_constant(&p1[i+1], j - (i + 1), &d2))
				return false;
			prev_approx_flag = approximate_roots;
			approximate_roots = true;
			if (calc(NULL, &d1, p1[i].token.operatr, d2)) {
				approximate_roots = prev_approx_flag;
				if (p1[i].token.operatr == POWER && !domain_check)
					return false;
				domain_check = false;
			} else {
				approximate_roots = prev_approx_flag;
				return false;
			}
		}
		*dp = d1;
		return true;
	}
	return false;
}

/*
 * Parse a constant complex number expression.
 *
 * This routine should be fixed to work with multiple constant expressions,
 * like the above get_constant() does.
 *
 * If successful return true with complex number in *cp.
 */
int
parse_complex(p1, n, cp)
token_type	*p1;	/* expression pointer */
int		n;	/* length of expression */
complexs	*cp;	/* pointer to returned complex number */
{
	int		j;
	int		imag_cnt = 0, plus_cnt = 0, times_cnt = 0;
	complexs	c;
	int		level2;

	if (get_constant(p1, n, &c.re)) {
		c.im = 0.0;
		*cp = c;
		return true;
	}
	c.re = 0.0;
	c.im = 1.0;
	for (j = n - 1; j >= 0; j--) {
		switch (p1[j].kind) {
		case CONSTANT:
			break;
		case VARIABLE:
			if (var_is_const(p1[j].token.variable, NULL))
				break;
			if (p1[j].token.variable != IMAGINARY)
				return false;
			imag_cnt++;
			break;
		case OPERATOR:
			level2 = p1[j].level;
			switch (p1[j].token.operatr) {
			case TIMES:
				if (++times_cnt > 1)
					return false;
				if (p1[j-1].level != level2 || p1[j+1].level != level2)
					return false;
				if (p1[j-1].kind == VARIABLE && p1[j-1].token.variable == IMAGINARY) {
					if (!get_constant(&p1[j+1], 1, &c.im))
						return false;
					continue;
				}
				if (p1[j+1].kind == VARIABLE && p1[j+1].token.variable == IMAGINARY) {
					if (!get_constant(&p1[j-1], 1, &c.im))
						return false;
					continue;
				}
				return false;
			case DIVIDE:
				if (++times_cnt > 1)
					return false;
				if (p1[j-1].level != level2 || p1[j+1].level != level2)
					return false;
				if (p1[j-1].kind == VARIABLE && p1[j-1].token.variable == IMAGINARY) {
					if (!get_constant(&p1[j+1], 1, &c.im))
						return false;
					c.im = 1.0 / c.im;
					continue;
				}
				return false;
			case MINUS:
				if (imag_cnt) {
					c.im = -c.im;
				}
			case PLUS:
				if (++plus_cnt > 1)
					return false;
				if (p1[j-1].level == level2 && get_constant(&p1[j-1], 1, &c.re)) {
					continue;
				}
				if (p1[j+1].level == level2 && get_constant(&p1[j+1], 1, &c.re)) {
					if (p1[j].token.operatr == MINUS)
						c.re = -c.re;
					continue;
				}
			}
		default:
			return false;
		}
	}
	if (imag_cnt != 1)
		return false;
	*cp = c;
	return true;
}
