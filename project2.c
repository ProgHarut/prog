#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int lineCount = 0;  // Global variable to keep track of the number of lines written.

/**
 * Signal handler function to handle the interrupt signal (Ctrl+C).
 * Prints the current line count when the interrupt signal is received.
 *
 * @param signal: The signal number.
 */
void handleInterruptSignal(int signal) {
    printf("Number of lines written: %d\n", lineCount);
}

/**
 * Writes N lines to a file with the given name, using the provided placeholder string.
 * The program can work in the background and in normal mode.
 * When the interrupt signal (Ctrl+C) is received, the program outputs the current line count.
 *
 * @param fileName: The name of the file to be filled.
 * @param numLines: The number of lines to be written to the file.
 * @param placeholder: The placeholder string to be used for each line.
 * @return: 0 if the program executed successfully, -1 otherwise.
 */
int fillFileWithLines(const char* fileName, int numLines, const char* placeholder) {
    FILE* file = fopen(fileName, "w");  // Open the file in write mode.

    if (file == NULL) {
        printf("Failed to open the file.\n");
        return -1;
    }

    // Register the signal handler for the interrupt signal (Ctrl+C).
    signal(SIGINT, handleInterruptSignal);

    for (int i = 0; i < numLines; i++) {
        fprintf(file, "%s\n", placeholder);  // Write the placeholder string to the file.
        lineCount++;  // Increment the line count.

        // Check if the program is running in the background.
        // If it is not, print the current line count.
        if (isatty(fileno(stdout))) {
            printf("Number of lines written: %d\n", lineCount);
        }
    }

    fclose(file);  // Close the file.

    return 0;
}

int main(int argc, char* argv[]) {
    // Check if the correct number of command line arguments are provided.
    if (argc != 4) {
        printf("Usage: %s <file_name> <num_lines> <placeholder>\n", argv[0]);
        return -1;
    }

    const char* fileName = argv[1];  // Get the file name from the command line arguments.
    int numLines = atoi(argv[2]);  // Get the number of lines from the command line arguments.
    const char* placeholder = argv[3];  // Get the placeholder string from the command line arguments.

    // Call the fillFileWithLines function to fill the file with lines.
    int result = fillFileWithLines(fileName, numLines, placeholder);

    return result;
}
