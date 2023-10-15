#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define N 4

void multiplyMatrices(int** matrix1, int** matrix2, int** result) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            result[i][j] = 0;
            for (int k = 0; k < N; k++) {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
}

int main() {
    // Create and fill the first matrix
    int** matrix1 = (int**)malloc(N * sizeof(int*));
    for (int i = 0; i < N; i++) {
        matrix1[i] = (int*)malloc(N * sizeof(int));
        for (int j = 0; j < N; j++) {
            matrix1[i][j] = rand() % 10;
        }
    }

    // Create and fill the second matrix
    int** matrix2 = (int**)malloc(N * sizeof(int*));
    for (int i = 0; i < N; i++) {
        matrix2[i] = (int*)malloc(N * sizeof(int));
        for (int j = 0; j < N; j++) {
            matrix2[i][j] = rand() % 10;
        }
    }

    // Create a pipe array for communication
    int pipes[N][2];

    // Create child processes equal to the size of N
    for (int i = 0; i < N; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("Pipe creation failed");
            exit(1);
        }

        pid_t pid = fork();

        if (pid == -1) {
            perror("Fork failed");
            exit(1);
        } else if (pid == 0) { // Child process
            close(pipes[i][0]); // Close the read end of the pipe

            // Redirect stdout to the write end of the pipe
            dup2(pipes[i][1], 1);

            close(pipes[i][1]); // Close the write end of the pipe

            // Execute the matrix multiplication command
            execlp("paste", "paste", "-d", " ", "-", NULL);

            exit(0);
        }
    }

    // Parent process
    int** result = (int**)malloc(N * sizeof(int*));
    for (int i = 0; i < N; i++) {
        result[i] = (int*)malloc(N * sizeof(int));
    }

    FILE* outputFile = fopen("output.txt", "w");
    if (outputFile == NULL) {
        perror("Error opening output file");
        exit(1);
    }

    for (int i = 0; i < N; i++) {
        pid_t pid = wait(NULL); // Wait for child process to finish

        int row[N];

        // Read the row from the pipe
        read(pipes[i][0], row, N * sizeof(int));

        close(pipes[i][0]); // Close the read end of the pipe

        for (int j = 0; j < N; j++) {
            matrix2[j][i] = row[j]; // Fill the column of matrix2 with the received row
        }

        multiplyMatrices(matrix1, matrix2, result);

        // Write the row of the result matrix to the output file
        for (int j = 0; j < N; j++) {
            dprintf(fileno(outputFile), "%d ", result[i][j]);
        }
        dprintf(fileno(outputFile), "\n");
    }

    close(fileno(outputFile));

    // Free allocated memory
    for (int i = 0; i < N; i++) {
        free(matrix1[i]);
        free(matrix2[i]);
        free(result[i]);
    }
    free(matrix1);
    free(matrix2);
    free(result);

    return 0;
}
