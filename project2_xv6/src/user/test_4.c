#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int pid;
    pid = getpid();

    int x1 = getfilenum(pid);

    int fd1 = open("ls", 0);

    int x2 = getfilenum(pid);

    fprintf(1, "XV6_TEST_OUTPUT %d %d\n", x1, x2);

    close(fd1);
    exit(0);
}
