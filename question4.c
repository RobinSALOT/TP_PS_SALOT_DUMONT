#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define WELCOME_MESSAGE "Welcome to the ENSEA Shell!\nTo exit, type 'exit'.\n"
#define PROMPT "enseash %% "

void welcome(void);
void writeResult(int, int);
int executeCommand(char *);

void welcome() {
    write(STDOUT_FILENO, WELCOME_MESSAGE, strlen(WELCOME_MESSAGE));
}

void writeResult(int exitCode, int signalNumber) {
    write(STDOUT_FILENO, "enseash [", 9);

    if (signalNumber >= 0) {
        // If signalNumber is non-negative, it's a signal
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

    write(STDOUT_FILENO, "] %% ", 5);
}

int executeCommand(char *command) {
    pid_t pid = fork();

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

        args[i] = NULL;
        if (strcmp(args[0], "exit") != 0) {
            // Execute the command only if it's not the 'exit' command
            execvp(args[0], args);
        }

        // Exit the child process
        exit(EXIT_SUCCESS);
    } else {
        // In the parent process, wait for the child to finish
        int status;
        waitpid(pid, &status, 0);

        // Write the result
        if (WIFEXITED(status)) {
            writeResult(WEXITSTATUS(status), -1);
        } else if (WIFSIGNALED(status)) {
            writeResult(-1, WTERMSIG(status));
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
