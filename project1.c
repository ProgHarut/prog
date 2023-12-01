#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

int linesWritten = 0;

void signalHandler(int signal) {
    printf("Number of lines written: %d\n", linesWritten);
}

void writeFile(const char* fileName, int numLines, const char* placeholder) {
    FILE* filePointer = fopen(fileName, "w");

    if (filePointer == NULL) {
        printf("Failed to open the file\n");
        exit(EXIT_FAILURE);
    }
   
    signal(SIGINT, signalHandler);

    for (int i = 0; i < numLines; i++) {
        fprintf(filePointer, "%s\n", placeholder);
        linesWritten++;

        fflush(filePointer); // Очищает буфер после каждой операции записи. Должно гарантировать, что переполнения не будет, и данные попадут в файл немедленно
    }

    fclose(filePointer);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s <file name> <number of lines> <placeholder string>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char* fileName = argv[1];
    int numLines = atoi(argv[2]);
    const char* placeholder = argv[3];

    writeFile(fileName, numLines, placeholder);

    return 0;
}
