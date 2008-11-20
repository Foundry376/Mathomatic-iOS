/*
 * Include file for the Mathomatic symbolic math library API.
 */

int matho_init(void);
int matho_process(char *input, const char **outputp);
int matho_parse(char *input, const char **outputp);
void clear_all(void);

extern int	cur_equation;			/* current equation space number (origin 0) */
