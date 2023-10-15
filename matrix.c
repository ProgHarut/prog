#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define N 4

void multiplyRowByColumn(int row[N], int col[N]) {
    int result = 0;
    for (int i = 0; i < N; i++) {
        result += row[i] * col[i];
    }
    write(STDOUT_FILENO, &result, sizeof(int));
}

int main() {
    int matrix1[N][N];
    int matrix2[N][N];
    int result[N][N];

    // Open the input files
    int fd1 = open("matrix1.txt", O_RDONLY);
    int fd2 = open("matrix2.txt", O_RDONLY);

    // Read the matrices from the files
    read(fd1, matrix1, N * N * sizeof(int));
    read(fd2, matrix2, N * N * sizeof(int));

    // Close the input files
    close(fd1);
    close(fd2);

    int pipes[N][2];

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
            close(pipes[i][1]); // Close the write end of the pipe

            int row[N], col[N];
            read(pipes[i][0], row, N * sizeof(int));
            read(pipes[i][0], col, N * sizeof(int));

            multiplyRowByColumn(row, col);

            exit(0);
        } else { // Parent process
            close(pipes[i][0]); // Close the read end of the pipe

            write(pipes[i][1], matrix1[i], N * sizeof(int));
            int col[N];
            for (int j = 0; j < N; j++) {
                col[j] = matrix2[j][i];
            }
            write(pipes[i][1], col, N * sizeof(int));

            close(pipes[i][1]); // Close the write end of the pipe
        }
    }

    // Wait for all child processes to finish and read the results
    for (int i = 0; i < N; i++) {
        wait(NULL);
        read(pipes[i][0], result[i], N * sizeof(int));
    }

    // Open the output file
    int fd3 = open("result.txt", O_WRONLY | O_CREAT, 0644);

    // Write the result matrix to the file
    for (int i = 0; i < N; i++) {
        write(fd3, result[i], N * sizeof(int));
    }

    // Close the output file
    close(fd3);

    return 0;
}
