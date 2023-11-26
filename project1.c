#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t running = 1;
volatile int total_lines_written = 0;

void signal_handler(int signo) {
    if (signo == SIGINT) {
        printf("\nTotal lines written: %d\n", total_lines_written);
        running = 0;
    }
}

void write_to_file(const char* filename, int N, const char* filler) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    for (int i = 0; i < N && running; i++) {
        fputs(filler, file);
        fputs("\n", file);
        total_lines_written++;
        usleep(100000);  // Имитация записи строки в файл
    }

    fclose(file);
    printf("Total lines written: %d\n", total_lines_written);
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

    write_to_file(filename, N, filler);

    return 0;
}
