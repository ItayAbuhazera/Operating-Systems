#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    
    char *goodbye = "Goodbye World xv6\n";
    write(1, goodbye, strlen(goodbye));
    exit(0);
}
