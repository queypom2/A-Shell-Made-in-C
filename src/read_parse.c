/* READING AND PARSING COMMAND INPUT */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "read_parse.h"

/* function to read command line */
char * read_input(void)
{
    char *input = NULL;
    size_t buff_size = 0;

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
/* taken and modified from brennan.io: https://brennan.io/2015/01/16/write-a-shell-in-c/ */
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
    /* go through token and add it to array tokens */
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