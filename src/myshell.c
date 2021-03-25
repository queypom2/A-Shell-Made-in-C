#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "myshell.h"
#include "builtin.h"
#include "execute.h"
#include "read_parse.h"

int main(int argc, char * argv[])
{
    char * path_to_shell;
    int buff_size;

    readlink("/proc/self/exe", path_to_shell, buff_size); /* find path of current shell executable "./myshell" */
    setenv("shell", path_to_shell, 1); /* changes "shell" environment string to full path of shell executable */

    /* start command promp depending if commands are should be taken from a file
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

/* prompt used when given a batchfile with commands in arguments */
void prompt_filename(char * input[])
{
    FILE * fn;
    char * line = NULL;
    size_t len = 0;
    char ** args;
    int command = 1; /* 0 = empty command, 1 = default, 2 = invalid command */

    fn = fopen(input[1], "r");
    if (fn == NULL) {
        printf("File does not exist");
        exit(EXIT_FAILURE);
    }

    while (getline(&line, &len, fn) != -1) {
        printf("%s => ", getenv("PWD")); /* Command prompt symbol with current directory shown */
        printf("=> ");
        printf("%s", line); /* print command feom file after prompt */
        args = split_input(line);
        command = check_command(args);

        /* If command is 2 throw an error and set command to 1 again */
        if (command == 2) {
            printf("Invalid command entered, please enter valid command\n");
            printf("Use help to read user manual\n");
            command = 1;
        }
        /* Command is 0 if empty command is entered, print a new line and prompt the user again */
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
        printf("%s\n", getenv("PWD")); /*shows current directory */
        printf("=> ");
        input = read_input();
        args = split_input(input);
        command = check_command(args);

        /* If command is 2 throw an error and set command to 1 again */
        if (command == 2) {
            printf("Invalid command entered, please enter valid command\n");
            printf("Use help to read user manual\n");
            command = 1;
        }
        /* Command is 0 if empty command is entered, print a new line and prompt the user again */
        else if (command == 0) {
            printf("\n");
            command = 1;
        }
    }
}


/* Check what command given and execute */
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