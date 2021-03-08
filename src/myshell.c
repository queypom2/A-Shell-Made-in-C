#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "myshell.h"


int main(int argc, char * argv[])
{
    char * path_to_shell;
    int buff_size;

    readlink("/proc/self/exe", path_to_shell, buff_size); /* find path of current shell executable "./myshell" */
    setenv("shell", path_to_shell, 1); /* changes "shell" environment string to full path of shell executable */

    /* start command prompt */
    prompt();

    free(path_to_shell);
    return 1;
}

void prompt(void)
{
    char * input;
    char ** args;
    int command = 1; /* 0 = empty command, 1 = default, 2 = invalid command */
    int i = 0;

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
            command == 1;
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

    token = strtok(input, SEPARATORS);
    while (token != NULL) {
    tokens[i] = token;
    i++;

    if (i >= buff_size) {
        buff_size += BUFFSIZE;
        tokens = realloc(tokens, buff_size * sizeof(char*));
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

/* function to check if arguments are built-in commands
and run commands if they are */
int builtin(char **args) {
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

/* ============================================================================================= */
/* BUILT-IN COMMANDS */

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
        printf("%s\n", environ[i++]);
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

int check_command(char **args)
{
    /* If entered command is whitespace */
    if (args[0] == NULL) {
        return 0;
    }

    if (builtin(args)) {
        return 1;
    }

    free(args);
    return 2;
}