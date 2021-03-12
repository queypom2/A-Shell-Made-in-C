============================================================================================================
Name: Mark Kevin Queypo

Student Number: 19404214

                                            Statement of Plagiarism

I hereby declare that all information in this assignment has been obtained and presented in accordance with academic rules and ethical conduct and the work I am submitting in the files, except where I have indicated is my own work.

============================================================================================================

NOTES: - The shell can takea batchfile of commands and run them using "bin/myshell batchfile" where batchfile
        is the file with a list of commands


                                            What is a Shell

A shell is essentially a macro processor that runs commands. Unix shells are considered to be both a command interpreter and a programming language. It is command interpreter as the user has access to GNU utilities, while being a programming language allows users to use these utilities. Files filled with commands can be created and become commands themselves. Shells can be used interactively or non-interactively. It can accept user input type from keyboard or from input from files.[1]

                                            BUILT-IN COMMANDS

Builtin commands are simple commands implemented to the shell itself. When simple command taken from the first string is entered.
The command executed directly  without invoking another program.


- dir
    Usage: dir
    Description: lists current directory

- cd
    Usage: cd <Optional Directory>
    Description: Changes directory to directory mentioned in second argument and change environment variable
                 "PWD" to changed directory path. If no directory given, It will print out current directory

- clr
    Usage: clr
    Description: This will clear screen of shell

- environ
    Usage: environ
    Description: Prints out environment variables

- echo
    Usage: echo <arg1> <arg2> ... <argn>
    Description: reads arguments after command and print them

- help
    Usage: help
    Description: Shows the user manual from manual direactory ("manual/readme.md)

- pause
    Usage: pause
    Description: Pauses shell until enter is press

- quit
    Usage: quit
    Description: Terminates the shell


                                            Program Execution and I/O Redirection

This shell supports the execution program by having the first word being the program path like example shown below:

- Program

The shell also supports I/O redirection where input and output can be redirected using special symbols. Redirection
permits commands' file handles to be opened and closed, duplicated, refer to different files and can modify the files
read from and write to. It can also be used to change file handles in the current shell environment. They can be
used with simple commands or with executing programs. Redirections are processed from left to right[2]. In this
shell the symbols use for redirection are "<", ">" and ">>" where:

- <

    Usage: Program < input
    Description: takes input and gives it to program

- >

    Usage: Command/Program > output
    Description: Output from command or program is sent to output

- >>

    Usage: Command/Program >> output
    Description: Output from command or program is appended to output

Note it is posible to use both "<" and ">" (or ">>) like below:

- Program < input > output

                                            Processes (Background)

In a Unix shell, processes revolves around two system calls: fork() and exec(), the shell translates commands into a series of sytem calls. When a process is created, the parent process calls fork() is cnd creates a child process which is a clone of the parent. The parent receives the child process's PID (Process ID) numberas it's return value for fork() while the child process is given a PID of "0". The system call exec() is then called which replaces the current process with different program. [3]

When programs call fork() and returns value of system call. The parent process creates children processes. It then calls wait() on the child process ID and waits until child process terminates.

The parent process can continue running in parallel with the child process if it does not call wait(). This creates a background process where more than one program can run simultaneously.

                                            Shell Environment

A shell maintains an environment that includes a set of variables defined by the login program, the system initialization file, and the user initialization files. There are variables that are defined by default. They are:

- Environment variables: The values in this variable are dynamically named and affect the way executed prrocesses will behave in a computer. They can be seen by useng the "environ command"

- Local variables: These variables only affect the current shell. [4]



                                            References

[1] Bash Reference Manual for Bash version 5.1 (pg1) -December 2020. From gnu.org:
https://www.gnu.org/software/bash/manual/bash.pdf

[2] Bash Reference Manual for Bash version 5.1 (pg35) -December 2020. From gnu.org:
https://www.gnu.org/software/bash/manual/bash.pdf

[3] CSE 451 Winter 2004 (operating systems) by Prof. Lazowska. From washington.edu:
https://courses.cs.washington.edu/courses/cse451/04wi/section/ab/notes/fork/

[4] System Administration Guide: Basic Administration (Chapter 4 Managing User Accounts and Groups (Overview)). From oracle.com:
https://docs.oracle.com/cd/E19120-01/open.solaris/819-2379/userconcept-26/index.html
