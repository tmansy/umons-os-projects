#include "kernel/types.h"
#include "user/user.h"

int main() {
    char buf[10];
    read(0, buf, 1);
    read(0, buf, 1);

    int c = getreadcount();
    printf("readcount = %d\n", c);

    exit(0);
}