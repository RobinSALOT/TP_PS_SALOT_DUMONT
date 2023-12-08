#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define WELCOME_MESSAGE "Welcome to the ENSEA Shell.\nTo exit, type 'exit'.\n"
#define PROMPT "enseash %% "

// Times
struct timespec start;
struct timespec end;

void welcome(void);
void writeResult(int, int, long);
int executeCommand(char *);

void welcome() {
    write(STDOUT_FILENO, WELCOME_MESSAGE, strlen(WELCOME_MESSAGE));
}

void writeResult(int exitCode, int signalNumber, long duration) {
    // Write the result to the standard output
    write(STDOUT_FILENO, "enseash [", 9);

    if (signalNumber >= 0) {
        // If signalNumber is positive, it's a signal
        char signalStr[2]; // Assuming single-digit signal numbers
        signalStr[0] = '0' + signalNumber;
        signalStr[1] = '\0';
        write(STDOUT_FILENO, "sign:", 5);
        write(STDOUT_FILENO, signalStr, 1);
    } else {
        // If signalNumber is negative, it's an exit code
        char exitStr[2]; // Assuming single-digit exit codes
        exitStr[0] = '0' + exitCode;
        exitStr[1] = '\0';
        write(STDOUT_FILENO, "exit:", 5);
        write(STDOUT_FILENO, exitStr, 1);
    }

    write(STDOUT_FILENO, "|", 1);

    // Convert the duration to milliseconds
    char durationStr[20];
    snprintf(durationStr, sizeof(durationStr), "%ldms", duration);
    write(STDOUT_FILENO, durationStr, strlen(durationStr));

    write(STDOUT_FILENO, "] %% ", 6);
}

int executeCommand(char *command) {
    pid_t pid = fork();
    
    // Measure the start time
    clock_gettime(CLOCK_MONOTONIC, &start);

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // In the child process

        // Tokenize the command and its arguments
        char *args[10]; // Adjust the size based on your needs
        char *token = strtok(command, " ");
        int i = 0;

        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }

        args[i] = NULL; // Null-terminate the argument list

        if (strcmp(args[0], "exit") != 0) {
            // Execute the command ONLY if it's not the 'exit' command

            execvp(args[0], args);

            // Exit the child process
            exit(EXIT_SUCCESS);
        }

        // Exit the child process if the command is 'exit'
        exit(EXIT_SUCCESS);
    } else {
        // In the parent process, wait for the child to finish
        int status;
        waitpid(pid, &status, 0);

        // Measure the end time
        clock_gettime(CLOCK_MONOTONIC, &end);

        // Calculate the duration in milliseconds
        long duration = (end.tv_sec-start.tv_sec)*1000 + (end.tv_nsec-start.tv_nsec)/1000000;

        // Write the result
        if (WIFEXITED(status)) {
            writeResult(WEXITSTATUS(status), -1, duration); // Assuming a constant duration for simplicity
        } else if (WIFSIGNALED(status)) {
            writeResult(-1, WTERMSIG(status), 5); // Assuming a constant duration for simplicity
        }
    }

    return EXIT_SUCCESS;
}

int main() {
    welcome();

    // We place the code in a while loop to continuously listen to the console inputs, instead of doing it only once!
    while (1) {
        write(STDOUT_FILENO, PROMPT, strlen(PROMPT));

        // Read the user's command
        char command[100];
        ssize_t bytesRead = read(STDIN_FILENO, command, sizeof(command)-1);

        // Check for CTRL+D (EOF)
        if (bytesRead == 0) {
            write(STDOUT_FILENO, "Bye bye...\n", 11);
            break;
        }

        // Null-terminate the string
        command[bytesRead-1] = '\0';

        // Check if the user wants to exit
        if (strcmp(command, "exit") == 0) {
            write(STDOUT_FILENO, "Bye bye...\n", 11);
            break;
        }

        // Execute the command and display the result
        executeCommand(command);
    }

    return EXIT_SUCCESS;
}
