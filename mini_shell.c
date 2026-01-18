/*
 * Minimal Unix-like Shell
 *
 * Commands / Operators:
 *  command        : Execute an external program
 *  cmd1 ; cmd2    : Execute multiple commands sequentially
 *  < file         : Redirect standard input from file
 *  > file         : Redirect standard output to file
 *  &              : Execute command in background
 *  quit           : Exit the shell
 */

#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <fcntl.h> 
#include <unistd.h> 
#define MAXARG 7 
void execute(char* args[], int argn); 
 
int main(void) { 
    char buf[256]; 
    char* args[MAXARG]; 
    char* s, * cmd; 
    char* save, * cmd_save; 
    int argn; 
    static const char delim[] = " \t\n"; 
    int pid, status; 
 
    while (1) { 
        printf("[shell] "); 
        gets(buf); 

        // Split input into commands separated by ';'
        cmd = strtok_r(buf, ";", &cmd_save); 
        while (cmd) { 
            argn=0; 

            // Tokenize command into arguments
            s = strtok_r(cmd, delim, &save);  
            while (s) { 
                args[argn++] = s; 
                s = strtok_r(NULL, delim, &save); 
            } 
            args[argn] = (char*)0; 

            // Built-in command
            if (!strcmp(args[0], "quit")) 
                exit(1); 
 
            execute(args, argn); 
 
            cmd = strtok_r(NULL, ";", &cmd_save); 
        } 
    } 
 
    exit(0); 
} 

// Execute a command with optional redirection and background support
void execute(char* args[], int argn) { 
    int input_redirect = -1, output_redirect = -1; 
    int background = 0; 
    char* input_file, *output_file; 
    int pid, status, fd; 

    // Parse redirection and background operators
    for (int i = 0; i < argn; i++) { 
        if (strcmp(args[i], "<") == 0) { 
            input_redirect = i; 
            input_file = args[i + 1]; 
            args[i] = NULL; 
        } 
        else if (strcmp(args[i], ">") == 0) { 
            output_redirect = i; 
            output_file = args[i + 1]; 
            args[i] = NULL; 
        } 
        else if (strcmp(args[i], "&") == 0) { 
            background = 1; 
            args[i] = NULL; 
        } 
    } 
 
    if ((pid = fork()) == -1) 
        perror("fork failed"); 

    // Child process: handle redirection and execute command
    if (pid == 0) { 
        if (input_redirect != -1) { 
            if ((fd = open(input_file, O_RDONLY)) < 0) { 
                perror("file open error"); 
                exit(1); 
            } 
            dup2(fd, 0); 
            close(fd); 
        } 
 
        if (output_redirect != -1) { 
            if ((fd = open(output_file, O_CREAT | O_TRUNC | O_WRONLY, 0644))<0) { 
                perror("file open error"); 
                exit(1); 
            } 
            dup2(fd, 1); 
            close(fd); 
        } 
 
        execvp(args[0], args); 
        fprintf(stderr, "명령어 실행 실패"); 
        exit(1); 
    } 

    // Parent process: wait for foreground job
    else {     
        if (background==0) { 
            pid=waitpid(pid, &status, 0); 
        } 
        else {
            fflush(stdout); 
        } 
    } 
} 
 
