/*
 * Include file for the Mathomatic symbolic math library API.
 */

int matho_init(void);
int matho_process(const char *input, char **outputp);
int matho_parse(const char *input, char **outputp);
void clear_all(void);

extern int	cur_equation;			/* current equation space number (origin 0) */
