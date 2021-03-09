/* Functions for Executing programs */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "execute.h"

/* function to execute programs (without redirection symbols) */
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

/* function for executing programs (with I/O redirection symbols) */
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