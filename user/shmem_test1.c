#include "kernel/types.h"
#include "user.h"
#include "kernel/syscall.h"

int main() {
    char *shmem = sbrk(4096); // Allocate shared memory
    printf("Allocated shared memory at %p\n", shmem);

    if (shmem == (void *) -1) {
        printf("Failed to allocate shared memory\n");
        exit(1);
    }
    int father_pid = getpid();
    printf("Father pid: %d\n", father_pid);

    if (fork() == 0) {
        // Child process
        printf("Child pid: %d\n", getpid());
        uint64 child_shmem_addr = map_shared_pages(father_pid, getpid(), (uint64) shmem, 4096);
        printf("Child: map_shared_pages returned %p\n", (void*)child_shmem_addr);
        if (child_shmem_addr == (uint64) -1) {
            printf("Failed to map shared memory\n");
            exit(1);
        }
        char *child_shmem = (char *) child_shmem_addr;
        printf("Child: mapped shared memory at %p\n", child_shmem);
        printf("Child: reading from shared memory: %s\n", child_shmem);
        exit(0);
    } else {
        // Parent process
        strcpy(shmem, "Hello child");
        printf("Parent: Added message to shared memory: 'Hello child'\n");
        wait(0); // Wait for the child process to finish
    }

    exit(0);
}
