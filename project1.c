#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

int total_lines_written = 0;

void signal_handler(int signo) {
    if (signo == SIGINT) {
        printf("\nTotal lines written: %d\n", total_lines_written);
        exit(0);
    }
}

void write_to_file(const char* filename, int N, const char* filler, int* lines_written) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    for (int i = 0; i < N; i++) {
        if (write(fileno(file), filler, strlen(filler)) == -1) {
            perror("Error writing to file");
            exit(1);
        }
        
        fputs("\n", file);
        (*lines_written)++;
        
        usleep(100000);  // Имитация записи строки в файл
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <filename> <N> <filler>\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1];
    int N = atoi(argv[2]);
    const char* filler = argv[3];

    signal(SIGINT, signal_handler);

    write_to_file(filename, N, filler, &total_lines_written);
    
    printf("Total lines written: %d\n", total_lines_written);

    return 0;
}
