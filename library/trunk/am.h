/*
 * Main include file for Mathomatic.  Can be edited.
 *
 * Copyright (C) 1987-2009 George Gesslein II.
 */

#ifndef	max
#define max(a, b)    (((a) > (b)) ? (a) : (b))		/* return the maximum of two values */
#endif
#ifndef	min
#define min(a, b)    (((a) < (b)) ? (a) : (b))		/* return the minimum of two values */
#endif

#ifndef	INFINITY
#define INFINITY	HUGE_VAL			/* the floating point, positive infinity constant */
#endif
#ifndef	isfinite
#define	isfinite(d)	finite(d)			/* true if d is finite (not infinity or NaN) */
#endif

#define	ARR_CNT(a)	((int) (sizeof(a)/sizeof(a[0])))	/* returns the number of elements in an array */
#define CLEAR_ARRAY(a)	memset(a, 0, sizeof(a))			/* sets all elements of an array to zero */

#define	MAX_K_INTEGER	1.0e15				/* maximum safely representable integer, 15 digits for doubles */

#define	blt(dst, src, cnt)	memmove((char *) (dst), (char *) (src), (size_t) (cnt))	/* memory copy function */
#define always_positive(power)	(fmod((double) (power), 2.0) == 0.0)	/* true if all real numbers raised to "power" result in positive, real numbers */

#if	I18N		/* internationalization: output in languages other than English using gettext(3) */
#define _(str)		gettext(str)	/* currently the support code for this incomplete and untested */
#else
#define _(str)		str		/* constant strings to be translated should be marked with this macro */
#endif

#define	STANDARD_SCREEN_COLUMNS	80			/* default number of columns of characters on the screen */
#define	STANDARD_SCREEN_ROWS	24			/* default number of lines on the screen */

#define	TMP_FILE	"/tmp/mathomatic.XXXXXX"	/* temporary file template for mkstemp(3) */

#define	PROMPT_STR	"-> "				/* user interface main prompt strings, preceded by the current equation number */
#define	HTML_PROMPT_STR	"&mdash;&gt; "

#define	MAX_CMD_LEN	(max(PATH_MAX, 500))		/* maximum command line length (not expression input), also max filename length */
#define	MAX_PROMPT_LEN	STANDARD_SCREEN_COLUMNS		/* maximum length of prompts */

/*
 * The following defines the maximum number of equation spaces that can be allocated.
 * This affects maximum memory usage.
 */
#ifndef	N_EQUATIONS
#if	HANDHELD
#define	N_EQUATIONS	20
#else
#define	N_EQUATIONS	100
#endif
#endif

/*
 * The following defines the default maximum mathematical expression size.
 * Expression arrays are allocated with this size by default,
 * there are 2 of these for each equation space (1 for LHS and 1 for RHS).
 * DEFAULT_N_TOKENS is linearly related to the actual memory usage of Mathomatic.
 * This should be made much smaller for handhelds and embedded systems.
 * Do not set to less than 100.
 */
#ifndef	DEFAULT_N_TOKENS
#if	HANDHELD
#define	DEFAULT_N_TOKENS	3000
#else
#define	DEFAULT_N_TOKENS	60000
#endif
#endif

#define	DIVISOR_SIZE		(DEFAULT_N_TOKENS / 2)	/* a nice maximum divisor size */

/*
 * All Mathomatic variables are referenced by the value in a C long int variable.
 * The actual name string is stored separately.
 */
#define	MAX_VAR_NAMES	8000			/* maximum number of variable names, keep this under (VAR_MASK - VAR_OFFSET) */
#define	MAX_VAR_LEN	100			/* maximum number of characters in variable names */

#define	MAX_VARS	min(DEFAULT_N_TOKENS / 4, 1000)	/* maximum number of unique variables handled in each equation */

#define	VAR_OFFSET	'A'			/* makes space for predefined variables */
#define	VAR_MASK	0x3fffL			/* mask for bits containing a reference to the variable name */
#define	VAR_SHIFT	14			/* number of bits set in VAR_MASK */
#define	SUBSCRIPT_MASK	63			/* mask for variable subscript after shifting VAR_SHIFT */
#define	MAX_SUBSCRIPT	(SUBSCRIPT_MASK - 1)	/* maximum variable subscript, currently only used for "sign" variables */

typedef	char	sign_array_type[MAX_SUBSCRIPT+2];	/* boolean array for generating unique "sign" variables */

enum kind_list {		/* kinds of expression tokens specified in the union below */
	CONSTANT,
	VARIABLE,
	OPERATOR
};

typedef union {
	double	constant;	/* internal storage for Mathomatic numerical constants */
	long	variable;	/* internal storage for Mathomatic variables */
/* Predefined special variables follow (order is important): */
#define	V_NULL		0L	/* null variable, should never be stored in an expression */
#define	V_E		1L	/* the symbolic, universal constant "e" or "e#" */
#define	V_PI		2L	/* the symbolic, universal constant "pi" or "pi#" */
#define	IMAGINARY	3L	/* the imaginary constant "i" or "i#" */
#define	SIGN		4L	/* for two-valued "sign" variables */
#define	MATCH_ANY	5L	/* match any variable (wild-card variable) */
#define	V_INTEGER_NAME	"integer"	/* name for the integer variable type */
	int	operatr;	/* internal storage for Mathomatic operators */
/* Valid operators follow (order doesn't matter), 0 is reserved for no operator: */
#define	PLUS		1	/* a + b */
#define	MINUS		2	/* a - b */
#define	TIMES		3	/* a * b */
#define	DIVIDE		4	/* a / b */
#define	MODULUS		5	/* a % b */
#define	POWER		6	/* a ^ b */
#define	FACTORIAL	7	/* a! */
#define	NEGATE		8	/* special parser operator not seen outside parser */
#define IDIVIDE		9	/* a // b */
} storage_type;

typedef	struct {		/* storage structure for each token in an expression */
	enum kind_list	kind;	/* kind of token */
	int		level;	/* level of parentheses, origin 1 */
	storage_type	token;	/* the actual token */
} token_type;

typedef struct {		/* qsort(3) data structure for sorting Mathomatic variables */
	long	v;		/* Mathomatic variable */
	int	count;		/* number of times the variable occurs */
} sort_type;

/* Debugging macros: */
#if	SILENT
#define	list_tdebug(level)
#define	side_debug(level, p1, n1)
#define debug_string(level, str)
#else
#define list_tdebug(level)		list_debug(level, tlhs, n_tlhs, trhs, n_trhs)
#define	side_debug(level, p1, n1)	list_debug(level, p1, n1, NULL, 0)
#define debug_string(level, str)	{ if (debug_level >= (level)) fprintf(gfp, "%s\n", str); }
#endif
