#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define N 4

void fillMatrix(int matrix[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrix[i][j] = rand() % 10;
        }
    }
}

void multiplyMatrices(int matrix1[N][N], int matrix2[N][N], int result[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            result[i][j] = 0;
            for (int k = 0; k < N; k++) {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
}

void writeMatrixToFile(int matrix[N][N], const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(file, "%d ", matrix[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

int main() {
    // Create and fill the first matrix
    int matrix1[N][N];
    fillMatrix(matrix1);
    writeMatrixToFile(matrix1, "matrix1.txt");

    // Create and fill the second matrix
    int matrix2[N][N];
    fillMatrix(matrix2);
    writeMatrixToFile(matrix2, "matrix2.txt");

    // Create pipe channels for communication
    int channels[N][2];

    // Create child processes equal to the size of N
    for (int i = 0; i < N; i++) {
        if (pipe(channels[i]) == -1) {
            perror("Pipe creation failed");
            exit(1);
        }

        pid_t pid = fork();

        if (pid == -1) {
            perror("Fork failed");
            exit(1);
        } else if (pid == 0) { // Child process
            close(channels[i][0]); // Close the read end of the pipe

            // Redirect stdout to the write end of the pipe
            dup2(channels[i][1], STDOUT_FILENO);

            close(channels[i][1]); // Close the write end of the pipe

            // Output the row from matrix1 to the pipe
            for (int j = 0; j < N; j++) {
                printf("%d ", matrix1[i][j]);
            }

            exit(0);
        }
    }

    // Parent process
    int result[N][N];

    int resultFile = open("result.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (resultFile == -1) {
        perror("Error opening result file");
        exit(1);
    }

    for (int i = 0; i < N; i++) {
        pid_t pid = wait(NULL); // Wait for child process to finish

        int column[N];

        // Read the column from the pipe
        for (int j = 0; j < N; j++) {
            read(channels[j][0], &column[j], sizeof(int));
        }

        close(channels[i][0]); // Close the read end of the pipe

        // Fill the column of matrix2 with the received column
        for (int j = 0; j < N; j++) {
            matrix2[j][i] = column[j];
        }

        multiplyMatrices(matrix1, matrix2, result);

        // Write the row of the result matrix to the output file
        for (int j = 0; j < N; j++) {
            dprintf(resultFile, "%d ", result[i][j]);
        }
        dprintf(resultFile, "\n");
    }

    close(resultFile);

    return 0;
}
