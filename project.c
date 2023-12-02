#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/file.h>

int linesWritten = 0;

void signalHandler(int signal) {
    printf("Number of lines written: %d\n", linesWritten);
}

void writeFile(const char* fileName, int numLines, const char* placeholder) {
    FILE* filePointer = fopen(fileName, "a+");

    if (filePointer == NULL) {
        perror("Failed to open the file\n");
        exit(EXIT_FAILURE);
    }

    if (flock(fileno(filePointer), LOCK_EX) == -1) {
        perror("Failed to lock file for exclusive use\n");
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
        perror("Failed to unlock file\n");
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
    int numLines = atoi(argv[2]);
    const char* placeholder = argv[3];

    writeFile(fileName, numLines, placeholder);

    return 0;
}
