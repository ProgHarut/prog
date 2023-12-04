#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/file.h>
#include <ctype.h>

int linesWritten = 0;

void signalHandler(int signal) {
    printf("Number of lines written: %d\n", linesWritten);
}

void writeFile(const char* fileName, int numLines, const char* placeholder) {
    FILE* filePointer = fopen(fileName, "a+");

    if (filePointer == NULL) {
        printf("Failed to open the file\n");
        exit(EXIT_FAILURE);
    }

    if (flock(fileno(filePointer), LOCK_EX) == -1) {
        printf("Failed to lock file for exclusive use\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, signalHandler);

    for (int i = 0; i < numLines; i++) {
        fprintf(filePointer, "%s\n", placeholder);
        linesWritten++;
       
        fflush(filePointer);
        usleep(100000);
    }

    if (flock(fileno(filePointer), LOCK_UN) == -1) {
        printf("Failed to unlock file\n");
        exit(EXIT_FAILURE);
    }

    fclose(filePointer);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s <filename> <number of lines> <placeholder>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char* fileName = argv[1];
    if (strstr(fileName, ".txt") == NULL) {
        printf("The provided file must be ended with .txt\n");
        exit(EXIT_FAILURE);
    }
   
    int numLines = atoi(argv[2]);
    if (numLines <= 0) {
        printf("The number of lines must be a positive integer\n");
        exit(EXIT_FAILURE);
    }
   
    char* placeholder = argv[3];
    while (*placeholder) {
        if (!isalnum(*placeholder) && !isspace(*placeholder)) {
            printf("The placeholder must consist of letters and numbers\n");
            exit(EXIT_FAILURE);
      }
        placeholder++;
    }

    writeFile(fileName, numLines, argv[3]);

    return 0;
}
