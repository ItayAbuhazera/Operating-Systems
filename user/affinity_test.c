#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    set_affinity_mask(0xA); // Set to CPU 0

    for(;;) {
    printf("Process %d running on CPU %d\n", getpid(), get_cpu_id());
    sleep(1);
    }

    exit(0,0);
}