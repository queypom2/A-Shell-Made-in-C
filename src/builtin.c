/* This is where everything related to builtin commands are */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include  "builtin.h"

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

/* read user manual with more filtered as specified */
void builtin_help(char **args)
{
    FILE * fn;
    FILE * fo;
    char * line = NULL;
    size_t len = 0;

    char * dir;
    char * path;

    /* for the help function this requires it to be in the bin directory
    So I need to use the shell environment variable to change to that directory*/
    path = getenv("shell");

    /* search for executable in shell environ and replace executable with empty string
    to to go to the directory */
    dir = strstr(path, "\\myshell");
    strncpy(dir, "", 1);
    chdir(path); /* change to that directory */

    /* if nothing after command like redirection symbols then output manual */
    if (args[1] == NULL) {
        system("more ../manual/readme.md");
    }

    /* for outputing help output to file */
    else if (!strcmp(args[1], ">")) {
        fn = fopen(args[2], "w");
        fo = fopen("../manual/readme.md", "r");
        if (fo == NULL) {
            printf("Error with file");
        }
        while(getline(&line, &len, fo) != -1) {
            fputs(line, fn);
        }
        fclose(fn);
        fclose(fo);
    }
    /* appending to file */
    else if (!strcmp(args[1], ">>")) {
        fn = fopen(args[2], "w");
        fo = fopen("../manual/readme.md", "r");
        if (fo == NULL) {
            printf("Error with file");
        }
        while(getline(&line, &len, fo) != -1) {
            fputs(line, fn);
        }
        fclose(fn);
        fclose(fo);
    }

    chdir(getenv("PWD")); /* when done change back to directory you were on */
    strcat(path, "\\myshell"); /* add the executable to environ shell again */

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
    DIR * tmp;
    FILE * fn;
    struct dirent *d;

    if (args[1] == NULL) {
        tmp = opendir(".");

        if (tmp == NULL) {
            perror("Cannot find directory");
        }

        while ((d = readdir(tmp))) {
            printf("%s\n", d->d_name);
        }
    }

    /* if given a directory to list */
    else if (args[1] != NULL && (strcmp(args[1], ">") && strcmp(args[1], ">>"))){
        tmp = opendir(args[1]);

        if (tmp == NULL) {
            perror("Cannot find directory");
        }

        /* if arguments after specified directory is a redirection symbol */
        if (args[2] != NULL && !strcmp(args[2], ">")) {
            /* writing to new file */
            fn = fopen(args[3], "w");
            while ((d = readdir(tmp))) {
                fprintf(fn, "%s\n", d->d_name);
            }
            fclose(fn);
        }

        else if (args[2] != NULL && !strcmp(args[2], ">>")) {
            /* appending to file */
            fn = fopen(args[2], "a");
            while ((d = readdir(tmp))) {
                fprintf(fn, "%s\n", d->d_name);
            }
            fclose(fn);
        }

        else {
            /* if no symbol found then just print it */
            while ((d = readdir(tmp))) {
                printf("%s\n", d->d_name);
            }
        }
    }

    /* check if it has a redirection symbol (Current directory)*/
    else if (args[1] != NULL && (!strcmp(args[1], ">") || !strcmp(args[1], ">>"))) {
        tmp = opendir(".");

        if (tmp == NULL) {
            perror("Cannot find directory");
        }

        if (!strcmp(args[1], ">")) {
            /* writing to new file */
            fn = fopen(args[2], "w");
            while ((d = readdir(tmp))) {
                fprintf(fn, "%s\n", d->d_name);
            }
            fclose(fn);
        }
        else {
            /* appending to file */
            fn = fopen(args[2], "a");
            while ((d = readdir(tmp))) {
                fprintf(fn, "%s\n", d->d_name);
            }
            fclose(fn);
        }
    }
}

/* prints environment strings */
void builtin_environ(char **args)
{
    extern char **environ;
    FILE * fn;
    int i = 0;

    /* For sending output to file) */
    if (args[1] != NULL && !strcmp(args[1], ">")) {
        /* writing */
        fn = fopen(args[2], "w");
        while (environ[i]){
            fprintf(fn, "%s\n", environ[i]);
            i++;
        }
        fclose(fn);
    }

    else if (args[1] != NULL && !strcmp(args[1], ">>")) {
        /* appending */
        fn = fopen(args[2], "a");
        while (environ[i]){
            fprintf(fn, "%s\n", environ[i]);
            i++;
        }
        fclose(fn);
    }

    else if (args[1] == NULL){
        /* If no redirection symbol, just print it out */
        while (environ[i]) {
            printf("%s\n", environ[i]);
            i++;
    }
    }
}

/* function that prints out arguments */
void builtin_echo(char **args)
{
    int i = 1;
    int j = 0;

    /* for sending/ appending ouput to output file */
    FILE * fn;

    /* check if args has a redirection symbol */
    while (args[i] != NULL && strcmp(args[i], ">") && strcmp(args[i], ">>")) {
        i++;
    }

    if (args[i] != NULL && !strcmp(args[i], ">")) {
        j = 1;
        fn = fopen(args[i + 1], "w"); /* file is after the redirect symbol */
        /* print everything before ">" which is at index i */
        while (j < i) {
            fputs(args[j], fn);
            fputs(" ", fn);
            j++;
        }
        fputs("\n", fn);
        fclose(fn);
    }
    /* appending */
    else if (args[i] != NULL && !strcmp(args[i], ">>")) {
        j = 1;
        fn = fopen(args[i + 1], "a"); /* file is after the redirect symbol */
        /* print everything before ">" which is at index i */
        while (j < i) {
            fputs(args[j], fn);
            fputs(" ", fn);
            j++;
        }
        fputs("\n", fn);
        fclose(fn);
    }

    /* if no redirection found print output */
    else {
        j = 1;
        while (args[j] != NULL) {
            printf("%s ", args[j]);
            j++;
        }
        printf("\n");
    }
}

/* function to pause operation of shell until Enter is pressed */
void builtin_pause(char **args)
{
    printf("Press enter to continue..");
    while (1) {
        if (getchar() == '\n') {
            break;
        }
    }
}

/* function to terminate shell */
void builtin_quit(char **args)
{
    exit(0);
}