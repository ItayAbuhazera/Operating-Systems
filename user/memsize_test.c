#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void)
{
  int initial_memsize = memsize();
  printf("Initial memory size: %d bytes\n", initial_memsize);

  // Allocate 20k more bytes of memory
  char *buffer = malloc(20 * 1024);
  int after_malloc_memsize = memsize();
  printf("Memory size after malloc: %d bytes\n", after_malloc_memsize);

  // Free the allocated memory
  free(buffer);
  int after_free_memsize = memsize();
  printf("Memory size after free: %d bytes\n", after_free_memsize);

  exit(0);
}