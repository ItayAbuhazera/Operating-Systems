# Execs /crypto_srv.
# This code runs in user space.

#include "syscall.h"

# exec(crypto_srv, argv)
.globl start
start:
        la a0, crypto_srv
        la a1, argv
        li a7, SYS_exec
        ecall

# for(;;) exit();
exit:
        li a7, SYS_exit
        ecall
        jal exit

# char crypto_srv[] = "/crypto_srv\0";
crypto_srv:
  .string "/crypto_srv\0"

# char *argv[] = { crypto_srv, 0 };
.p2align 2
argv:
  .long crypto_srv
  .long 0
