#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int pid, pid2, rc;
    pid = getpid();

    int x1 = getfilenum(pid); // -> 3

    int p1[2];
    rc = pipe(p1); // Using a pipe to wait for an event in the child
    if (rc < 0) {
        fprintf(2, "pipe failed\n");
        exit(1);
    }
    int x2 = getfilenum(pid); // A pipe opens two files! -> 5

    int p2[2];
    rc = pipe(p2); // Using a pipe to wait for an event in the parent
    if (rc < 0) {
        fprintf(2, "pipe failed\n");
        exit(1);
    }
    int x3 = getfilenum(pid); // -> 7

    pid2 = fork();
    if (pid2 == 0) {
        int fd3 = open("ls", 0); // Also opens a normal file
        if (fd3 < 0) {
            fprintf(2, "open failed\n");
        }
        write(p1[1], "a", 1);

        char c_child;
        read(p2[0], &c_child, 1); // Wait until parent has queried the filenum
        close(fd3);
        exit(0);
    }
    else {
        char c_parent;
        read(p1[0], &c_parent, 1); // Waiting for the child to open things
        int x4 = getfilenum(pid2); // -> 8
        write(p2[1], "a", 1);

        wait(0); // Make sure the child has exited
        int x5 = getfilenum(pid2); // -> -1
        // Could actually return something != -1 if another process with the 
        // same pid is created in the meantime!

        fprintf(1, "XV6_TEST_OUTPUT %d %d %d %d %d\n", x1, x2, x3, x4, x5);
        exit(0);
    }
}
