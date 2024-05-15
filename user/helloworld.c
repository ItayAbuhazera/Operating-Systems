#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main()
{
  char *hello = "Hello World xv6\n";
    write(1, hello, strlen(hello));
    exit(0);

}
