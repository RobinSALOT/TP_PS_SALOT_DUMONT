#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define WELCOME_MESSAGE "Welcome to the ENSEA Shell!\nTo exit, type 'exit'.\n"
#define PROMPT "enseash %% "

void welcome(void);
int executeCommand(char*);

void welcome() {
    write(STDOUT_FILENO, WELCOME_MESSAGE, strlen(WELCOME_MESSAGE));
}

int executeCommand(char *command) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // In the child process
        char *argv[] = {command, NULL};
   	    if (execlp(command, command, (char *) NULL) == -1) {
      	    perror("execlp");
	        exit(EXIT_FAILURE);
	    }
        exit(EXIT_SUCCESS);
    } else {
        // In the parent process, wait for the child to finish
        wait(NULL);
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

        // Remove the newline ('\n') character at the end of the command
        command[strcspn(command, "\n")] = '\0';

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

