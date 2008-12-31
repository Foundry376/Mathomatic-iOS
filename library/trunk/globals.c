/*
 * Mathomatic global variables and arrays.
 * C initializes global variables and arrays to zero by default.
 * This is required for proper operation.
 *
 * Copyright (C) 1987-2008 George Gesslein II.
 *
 * Most global variables for Mathomatic are defined here and duplicated in "externs.h".
 */

#include "includes.h"

int		n_tokens = DEFAULT_N_TOKENS;	/* maximum size of expressions, must only be set during startup */

int		n_equations,			/* number of equation spaces allocated */
		cur_equation;			/* current equation space number (origin 0) */

/* expression storage pointers and current length variables (they go together) */
token_type	*lhs[N_EQUATIONS],		/* The Left Hand Sides of equation spaces */
		*rhs[N_EQUATIONS];		/* The Right Hand Sides of equation spaces */

int		n_lhs[N_EQUATIONS],		/* number of tokens in each lhs[], 0 means equation space is empty */
		n_rhs[N_EQUATIONS];		/* number of tokens in each rhs[], 0 means not an equation */

token_type	*tlhs,				/* LHS during solve and temporary storage for expressions */
		*trhs,				/* RHS during solve and temporary storage for expressions */
		*tes,				/* temporary equation side, used only in commands */
		*scratch;			/* very temporary storage for expressions */

int		n_tlhs,				/* number of tokens in tlhs */
		n_trhs,				/* number of tokens in trhs */
		n_tes;				/* number of tokens in tes */

token_type	zero_token,			/* the universal constant 0.0 as an expression */
		one_token;			/* the universal constant 1.0 as an expression */

/* Set options with their initial values. */
int		precision = 14;				/* the display precision for doubles (number of digits) */
int		case_sensitive_flag = true;		/* "set case_sensitive" flag */
int		factor_int_flag;			/* factor integers when displaying expressions */
int		display2d = true;			/* "set display2d" flag */
int		preserve_surds = true;			/* set option to preserve roots like (2^.5) */
int		modulus_mode = 2;				/* true for mathematically correct modulus */
volatile int	screen_columns = STANDARD_SCREEN_COLUMNS;	/* screen width of the terminal; 0 = infinite */
volatile int	screen_rows = STANDARD_SCREEN_ROWS;		/* screen height of the terminal; 0 = infinite */
int		finance_option;				/* for displaying dollars and cents */
int		autosolve = true;			/* Allows solving by typing the variable name at the prompt */
int		autocalc = true;			/* Allows automatically calculating a numerical expression */
char		special_variable_characters[256] = "\\"; /* user defined characters for variable names, 0 terminated */
int		integer_coefficients = false;		/* if true, factor out the GCD of integer coefficients */
int		right_associative_power;		/* if true, evaluate power operators right to left */
int		negate_high_precedence = true;		/* if true, negation (-x) has the highest precedence */
int		power_starstar;				/* if true, display power operator as "**", otherwise "^" */
#if	!SILENT
int		debug_level;				/* current debug level */
#endif

/* variables having to do with color mode */
int		color_flag = true;		/* "set color" flag, true for color mode */
int		bold_colors;			/* "set bold_colors" flag, true for bold colors if ANSI color mode */
int		cur_color = -1;			/* current color on the terminal */
int		html_flag;			/* true for HTML output mode */

/* epsilon constants */
double		small_epsilon	= 0.000000000000005;	/* for small round-off errors */
double		epsilon		= 0.00000000000005;	/* for larger, accumulated round-off errors */

/* string variables */
char		*prog_name = "mathomatic";	/* name of this program */
char		*var_names[MAX_VAR_NAMES];	/* index for storage of variable name strings */
char		var_str[MAX_VAR_LEN+80];	/* temp storage for listing a variable name */
char		prompt_str[MAX_PROMPT_LEN];	/* temp storage for the prompt string */
#if	CYGWIN
char		*dir_path;			/* directory path to the executable, only set and used in CYGWIN version */
#endif
#if	READLINE
char		*last_history_string;
char		*history_filename;
char		history_filename_storage[PATH_MAX];
#endif

/* The following are for integer factoring (filled by factor_one()): */
double		unique[64];		/* storage for the unique prime factors */
int		ucnt[64];		/* number of times the factor occurs */
int		uno;			/* number of unique factors stored in unique[] */

/* misc. variables */
sign_array_type	sign_array;		/* for keeping track of unique "sign" variables */
FILE		*gfp;			/* global output file pointer, for dynamically redirecting Mathomatic output */
jmp_buf		jmp_save;		/* for setjmp(3) to longjmp(3) to when an error happens deep within this code */
int		test_mode;		/* test mode flag (-t) */
int		quiet_mode;		/* quiet mode (-q, don't display prompts) */
int		echo_input;		/* if true, echo input and don't use readline */
int		readline_enabled = true;	/* set to false (-r) to disable readline */
int		partial_flag;		/* normally true for partial unfactoring, false for "unfactor fully" */
int		symb_flag;		/* true during "simplify symbolic" */
int		high_prec;		/* flag to output constants in higher precision (used when saving equations) */
int		input_column;		/* current column number on the screen at the beginning of a parse */
int		sign_cmp_flag;		/* true when all "sign" variables are to compare equal */
int		domain_check;		/* flag to track domain errors in the pow() function */
int		approximate_roots;	/* true if in calculate command (force approximation of roots like (2^.5)) */
volatile int	abort_flag;		/* if true, abort current operation */

/* library variables go here */
const char	*error_str;		/* last error string */
char		*result_str;		/* returned result when using as library */
