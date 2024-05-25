#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    set_affinity_mask(0x1); // Set to CPU 0

    for(;;) {
    printf("Process %d running\n", getpid());
    sleep(100);
    }

    exit(0,0);
}