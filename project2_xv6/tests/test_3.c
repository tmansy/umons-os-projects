#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define N 15                 // Number of child processes
#define READS_PER_PROC 10000 // Number of reads each process performs

int main(void) {
    int i, pid;
    int x1 = getreadcount();

    for (i = 0; i < N; i++) {
        pid = fork();
        if (pid == 0) {
            // Each child process performs READS_PER_PROC read calls
            int j;
            for (j = 0; j < READS_PER_PROC; j++) {
                char buf[1];
                read(4, buf, 1);
            }
            exit(0);
        }
    }

    for (i = 0; i < N; i++) {
        wait(0);
    }
    int x2 = getreadcount();
    fprintf(1, "XV6_TEST_OUTPUT %d\n", x2 - x1);
    exit(0);
}
