void prompt(void);
char * read_input(void);
char **split_input(char *input);
int check_command(char **input);
int builtin(char **input);
void prompt_filename(char *input[]);


void builtin_help(char **args);
void builtin_clear(char **args);
void builtin_cd(char **args);
void builtin_dir(char **args);
void builtin_environ(char **args);
void builtin_echo(char **args);
void builtin_pause(char **args);
void builtin_quit(char **args);

int execute(int amp, char **args);
int execute_io(char **args);