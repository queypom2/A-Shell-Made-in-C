#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "myshell.h"

int main(int argc, char * argv[])
{
    char * path_to_shell;
    int buff_size;

    readlink("/proc/self/exe", path_to_shell, buff_size); /* find path of current shell executable "./myshell" */
    setenv("shell", path_to_shell, 1); /* changes "shell" environment string to full path of shell executable */

    /* start command promp depending if commands are from a file
    or user input */
    if (argc > 1) {
        prompt_filename(argv);
    }
    else {
        prompt();
    }

    free(path_to_shell);
    return 1;
}

/* prompt used when given a file with commands in arguments */
void prompt_filename(char * input[])
{
    FILE * fn;
    char * line = NULL;
    ssize_t len = 0;
    char ** args;
    int command = 1; /* 0 = empty command, 1 = default, 2 = invalid command */

    fn = fopen(input[1], "r");
    if (fn == NULL) {
        printf("File does not exist");
        exit(EXIT_FAILURE);
    }

    while (getline(&line, &len, fn) != -1) {
        printf("=> ");
        printf("%s", line);
        args = split_input(line);
        command = check_command(args);

        if (command == 2) {
            printf("Invalid command entered, please enter valid command\n");
            printf("Use help to read user manual\n");
            command = 1;
        }
        else if (command == 0) {
            printf("\n");
            command = 1;
        }
    }

    fclose(fn);
    if (line) {
        free(line);
    }
    exit(EXIT_SUCCESS);
}

/* Normal prompt that takes user inputs */
void prompt(void)
{
    char * input;
    char ** args;
    int command = 1; /* 0 = empty command, 1 = default, 2 = invalid command */

    while (1) {
        printf("=> ");
        input = read_input();
        args = split_input(input);
        command = check_command(args);

        if (command == 2) {
            printf("Invalid command entered, please enter valid command\n");
            printf("Use help to read user manual\n");
            command = 1;
        }
        else if (command == 0) {
            printf("\n");
            command = 1;
        }
    }
}

/* ============================================================================================= */
/* READING AND PARSING COMMAND INPUT */

/* function to read command line */
char * read_input(void)
{
    char *input = NULL;
    int i = 0;
    ssize_t buff_size = 0;

    /* getline() gets input store it in input and updates buff_size and resizes
    it when buf_size is too small */
    if (getline(&input, &buff_size, stdin) == -1) {

        /* if input detected is EOF. terminate the program */
        if (feof(stdin)) {
            exit(EXIT_SUCCESS);
        }
        else {
            perror("failed to read line");
            exit(EXIT_FAILURE);
        }
    }
    return input;
}

/* function to split input into an array */
#define BUFFSIZE 64
#define SEPARATORS " \t\r\n\a"
char **split_input(char *input)
{
    int buff_size = BUFFSIZE;
    int i = 0;
    char **tokens = calloc(buff_size, sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "Failed to allocate memory.\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(input, SEPARATORS); /* splits the input into an array by whitespace */
    while (token != NULL) {
    tokens[i] = token;
    i++;

    if (i >= buff_size) {
        buff_size += BUFFSIZE;
        tokens = realloc(tokens, buff_size * sizeof(char*)); /* reallocate more memory if needed */
        if (!tokens) {
            fprintf(stderr, "Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }
    }

    token = strtok(NULL, SEPARATORS);
    }
    tokens[i] = NULL;
    return tokens;
}

/* ============================================================================================= */



/* ============================================================================================= */
/* BUILT-IN COMMANDS */

/* function to check if arguments are built-in commands
and run commands if they are */
int builtin(char **args)
{
    /* an array of commands */
    char * commands[8] = {
        "help",
        "clr",
        "cd",
        "dir",
        "environ",
        "echo",
        "pause",
        "quit"
    };

    /* function pointer type */
    typedef void (*f)(char **);

    /* An array of buil-in functions */
    f command_array[8] = {
        &builtin_help,
        &builtin_clear,
        &builtin_cd,
        &builtin_dir,
        &builtin_environ,
        &builtin_echo,
        &builtin_pause,
        &builtin_quit
    };

    int i;

    /* checks the command list if it matches */
    for (i = 0; i < 8; i++) {
        if (!strcmp(commands[i], args[0])) {
            break;
        }
    }

    /* if it's over the number of list of commands then it's not
    a built-in command */
    if (i >= 8) {
        return 0;
    }

    /* launch the function asoociated with enetered command from array */
    command_array[i](args);
    return 1;
}


/* read user manual */
void builtin_help(char **args)
{
    system("more ../manual/readme.md");
}

/* clear command line */
void builtin_clear(char **args)
{
    system("clear");
}

/* change directory */
void builtin_cd(char **args)
{
    if (args[1] == NULL) { /* if no arguments report current directory */
        printf("%s\n", getcwd(NULL,0));
    }
    else {
        if (chdir(args[1]) != 0) {
            perror("changing directory unsuccessful");
        }
    }
    setenv("PWD", getcwd(NULL, 0), 1); /* Change environment variable "PWD" to new directory" */
}

/* list current directory */
void builtin_dir(char **args)
{
    char start[] = "ls -al ";

    if (args[1] != NULL) {
        char * full = calloc(strlen(start) + strlen(args[1]), sizeof(char*));

        if (full == NULL) {
            printf("Memory not allocated.\n");
        }
        else {
            strcat(full, start);
            strcat(full, args[1]);
            system(full);
            free(full);
        }
    }

    else {
        system(start);
    }
}

/* prints environment strings */
void builtin_environ(char **args)
{
    extern char **environ;
    int i = 0;

    while (environ[i]) {
        printf("%s\n", environ[i]);
        i++;
    }
}

/* function that prints out arguments */
void builtin_echo(char **args)
{
    int i = 1;

    while (args[i] != NULL) {
        printf("%s ", args[i]);
        i++;
    }
    printf("\n");
}

/* function to pause operation of shell until Enter is pressed */
void builtin_pause(char **args)
{
    while (1) {
        if (getchar() == '\n') {
            break;
        }
        else {
            sleep(1);
        }
    }
}

/* function to terminate shell */
void builtin_quit(char **args)
{
    exit(0);
}

/* ============================================================================================= */


/* ============================================================================================= */
/* Executing programss */

int execute(int amp, char ** args)
{
    pid_t pid;
    int status;

    pid = fork();

    /* child process */
    if (pid == 0) {
        /* execute the program */
        if (execvp(args[0], args) < 0) {
            /* If execution failed throw error message and continue prompt */
            perror("could not execute program");
            return 1;
        }

    }

    /* forking error */
    else if (pid < 0) {
        perror("forking error");
        return 1;
    }

    /* parent */
    else {
        /* wait forchild to execute if there is an ampersand */
        if (amp == 0) {
            waitpid(pid, &status, WUNTRACED);
        }
    }
    return 1;
}

/* function for executing programs with I/O redirection */
int execute_io(char ** args)
{
    pid_t pid;
    int status;
    FILE * fn;
    FILE * fo;

    pid = fork();

    /* child process */
    if (pid == 0) {
        /* reading input from file and writing output to output file */
        if (!strcmp(args[1], "<") && !strcmp(args[3], ">")) {
            fn = freopen(args[2], "r", stdin); /* change stdin stream with file for reading */
            fo = freopen(args[4], "w", stdout); /* change stdout stream with file for writing output */
            if (execvp(args[0], args) < 0) {
                /* If execution failed throw error message and continue prompt */
                perror("could not execute program");
                return 1;
            }
            fclose(fn);
            fclose(fo);
        }
        /* reading input from file and appending output to output file */
        else if (!strcmp(args[1], "<") && !strcmp(args[3], ">>")) {
            fn = freopen(args[2], "r", stdin);
            fo = freopen(args[4], "a", stdout);
            if (execvp(args[0], args) < 0) {
                /* If execution failed throw error message and continue prompt */
                perror("could not execute program");
                return 1;
            }
            fclose(fn);
            fclose(fo);
        }
        /* reading input from file only */
        else if (!strcmp(args[1], "<")) {
            fn = freopen(args[2], "r", stdin);
            if (execvp(args[0], args) < 0) {
                /* If execution failed throw error message and continue prompt */
                perror("could not execute program");
                return 1;
            }
            fclose(fn);
        }
        /* writing input to file only */
        else if (!strcmp(args[1], ">")) {
            fn = freopen(args[2], "w", stdout);
            if (execvp(args[0], args) < 0) {
                /* If execution failed throw error message and continue prompt */
                perror("could not execute program");
                return 1;
            }
            fclose(fn);
        }
        /* appending output to file */
        else if (!strcmp(args[1], ">>")) {
            fn = freopen(args[2], "a", stdout);
            if (execvp(args[0], args) < 0) {
                /* If execution failed throw error message and continue prompt */
                perror("could not execute program");
                return 1;
            }
            fclose(fn);
        }
    }

    /* forking error */
    else if (pid < 0) {
        perror("forking error");
        return 1;
    }

    /* parent */
    else {
        /* wait for child to execute */
        waitpid(pid, &status, WUNTRACED);
    }
    return 1;
}

/* ============================================================================================= */


int check_command(char **args)
{
    /* If entered command is whitespace */
    if (args[0] == NULL) {
        return 0;
    }

    if (builtin(args)) {
        return 1;
    }

    /* Find out if file is an executable */
    struct stat sb;
    if (stat(args[0], &sb) == 0 && sb.st_mode & S_IXUSR) {
        /* check if array size is less than 3 for I/O redirection*/
        if (args[2] == NULL) {
            if (args[1] == NULL) {
                return execute(0, args);
            }
            else {
                return execute(1, args);
            }
        }
        /* If args bigger than 2 then it might be program with I/O redirection */
        else {
            return execute_io(args);
        }
    }

    free(args);
    return 2;
}