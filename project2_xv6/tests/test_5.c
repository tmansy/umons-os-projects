#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int pid;
    pid = getpid();

    int x1 = getfilenum(pid);

    int fd1 = open("ls", 0);
    if(fd1 < 0) {
        fprintf(2, "open failed\n");
    }
    int x2 = getfilenum(pid);

    int fd2 = open("cat", 0);
    if(fd2 < 0) {
        fprintf(2, "open failed\n");
    }
    int x3 = getfilenum(pid);

    close(fd1);
    int x4 = getfilenum(pid);
    
    close(fd2);
    int x5 = getfilenum(pid);

    fprintf(1, "XV6_TEST_OUTPUT %d %d %d %d %d\n", x1, x2, x3, x4, x5);
    exit(0);
}
