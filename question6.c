#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define WELCOME_MESSAGE "Welcome to the ENSEA Shell.\nTo exit, type 'exit'.\n"
#define PROMPT "enseash %% "

void welcome(void);
void executeCommand(char *);

void welcome() {
    write(STDOUT_FILENO, WELCOME_MESSAGE, strlen(WELCOME_MESSAGE));
}

void executeCommand(char *command) {
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

        args[i] = NULL; // Null-terminate the argument list

        if (strcmp(args[0], "exit") != 0) {
            // Execute the command only if it's not the 'exit' command
            execvp(args[0], args);

            // Exit the child process if execvp fails
            exit(EXIT_FAILURE);
        }

        // Exit the child process if the command is 'exit'
        exit(EXIT_SUCCESS);
    } else {
        // In the parent process, wait for the child to finish
        waitpid(pid, NULL, 0);
    }
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

        // Execute the command
        executeCommand(command);
    }

    return EXIT_SUCCESS;
}
