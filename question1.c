#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define WELCOME_MESSAGE "Welcome to the ENSEA Shell!\nTo exit, type 'exit'.\n"
#define PROMPT "enseash %% "

void welcome(void);

void welcome() {
    write(STDOUT_FILENO, WELCOME_MESSAGE, strlen(WELCOME_MESSAGE));
}

int main() {
    welcome();

    // We place the code in a while loop to continuously listen to the console inputs, instead of doing it only once!
    while (1) {
        write(STDOUT_FILENO, PROMPT, strlen(PROMPT));

        // Read the user's command
        char command[100];
        ssize_t bytesRead = read(STDIN_FILENO, command, sizeof(command)-1);

        // Remove the newline ('\n') character at the end of the command
        command[strcspn(command, "\n")] = '\0';

        // Check if the user wants to exit
        if (strcmp(command, "exit") == 0) {
            break;
        }

        // Display a message to simulate the execution of the command
        write(STDOUT_FILENO, "Executed: ", 24);
        write(STDOUT_FILENO, command, strlen(command));
        write(STDOUT_FILENO, "\n", 1);
    }

    return EXIT_SUCCESS;
}

