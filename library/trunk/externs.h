/*
 * Mathomatic global variable extern definitions, from file "globals.c".
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

extern int		n_tokens;
extern int		n_equations;
extern int		cur_equation;

extern token_type	*lhs[N_EQUATIONS];
extern token_type	*rhs[N_EQUATIONS];

extern int		n_lhs[N_EQUATIONS];
extern int		n_rhs[N_EQUATIONS];

extern token_type	*tlhs;
extern token_type	*trhs;
extern token_type	*tes;

extern int		n_tlhs;
extern int		n_trhs;
extern int		n_tes;

extern token_type	*scratch;

extern token_type	zero_token;
extern token_type	one_token;

extern int		precision;
extern int		case_sensitive_flag;
extern int		factor_int_flag;
extern int		display2d;
extern int		fractions_display;
extern int		approximate_roots;
extern int		preserve_surds;
extern int		rationalize_denominators;
extern int		modulus_mode;
extern volatile int	screen_columns;
extern volatile int	screen_rows;
extern int		finance_option;
extern int		autosolve;
extern int		autocalc;
extern int		autoselect;
extern char		special_variable_characters[256];
extern char		plot_prefix[256];
extern int		integer_coefficients;
extern int		right_associative_power;
extern int		power_starstar;
extern int		debug_level;
extern int		domain_check;
extern int		color_flag;
extern int		bold_colors;
extern int		cur_color;
extern int		html_flag;
extern int		readline_enabled;
extern int		partial_flag;
extern int		symb_flag;
extern int		symblify;
extern int		high_prec;
extern int		input_column;
extern int		sign_cmp_flag;
extern double		small_epsilon;
extern double		epsilon;

extern char		*prog_name;
extern char		*var_names[MAX_VAR_NAMES];
extern char		var_str[MAX_VAR_LEN+80];
extern char		prompt_str[MAX_PROMPT_LEN];
extern char		rc_file[PATH_MAX];

extern char		*dir_path;
extern char		*last_history_string;
extern char		*history_filename;
extern char		history_filename_storage[PATH_MAX];

extern double		unique[];
extern int		ucnt[];
extern int		uno;

extern sign_array_type	sign_array;
extern FILE		*default_out;
extern FILE		*gfp;
extern jmp_buf		jmp_save;
extern int		eoption;
extern int		test_mode;
extern int		demo_mode;
extern int		quiet_mode;
extern int		echo_input;
extern volatile int	abort_flag;
extern int		pull_number;
extern int		security_level;
extern int		repeat_flag;

extern char		*result_str;
extern int		result_en;
extern const char	*error_str;
extern const char	*warning_str;
