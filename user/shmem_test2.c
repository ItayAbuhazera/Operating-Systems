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

    printf("Parent: Checking size before mapping\n");
    char *size_before_mapping = sbrk(0);
    printf("Parent: Size before mapping: %p\n", size_before_mapping);

    if (fork() == 0) {
        // Child process
        printf("Child pid: %d\n", getpid());

        printf("Child: Checking size before mapping\n");
        char *size_before_mapping_child = sbrk(0);
        printf("Child: Size before mapping: %p\n", size_before_mapping_child);

        uint64 child_shmem_addr = map_shared_pages(father_pid, getpid(), (uint64) shmem, 4096);
        printf("Child: map_shared_pages returned %p\n", (void*)child_shmem_addr);
        if (child_shmem_addr == (uint64) -1) {
            printf("Failed to map shared memory\n");
            exit(1);
        }

        printf("Child: Checking size after mapping\n");
        char *size_after_mapping_child = sbrk(0);
        printf("Child: Size after mapping: %p\n", size_after_mapping_child);

        char *child_shmem = (char *) child_shmem_addr;
        printf("Child: mapped shared memory at %p\n", child_shmem);
        printf("Child: reading from shared memory: %s\n", child_shmem);

        printf("Child: Unmapping shared memory\n");
        int unmap_result = unmap_shared_pages(getpid(), (uint64) child_shmem, 4096);
        if (unmap_result == -1) {
            printf("Failed to unmap shared memory\n");
        }

        printf("Child: Checking size after unmapping\n");
        char *size_after_unmapping_child = sbrk(0);
        printf("Child: Size after unmapping: %p\n", size_after_unmapping_child);

        exit(0);
    } else {
        // Parent process
        strcpy(shmem, "Hello child");
        wait(0);

        printf("Parent: Checking size after child process finished\n");
        char *size_after_child = sbrk(0);
        printf("Parent: Size after child process finished: %p\n", size_after_child);

        printf("Parent: Unmapping shared memory\n");
        int unmap_result = unmap_shared_pages(father_pid, (uint64) shmem, 4096);
        if (unmap_result == -1) {
            printf("Failed to unmap shared memory\n");
        }

        printf("Parent: Checking size after unmapping\n");
        char *size_after_unmapping = sbrk(0);
        printf("Parent: Size after unmapping: %p\n", size_after_unmapping);

    }

    exit(0);
}
