#include "kernel/types.h"
#include "user/user.h"
#include "kernel/crypto.h"
#include <stdint.h>
#include <fcntl.h> // Include this for O_RDWR definition

#define CRYPTO_OP_TYPE_ENCRYPT 1
#define CRYPTO_OP_TYPE_DECRYPT 2
#define CONSOLE 1
#define CRYPTO_OP_STATE_INIT 1
#define CRYPTO_OP_STATE_DONE 2
#define CRYPTO_OP_STATE_ERROR 3

void process_crypto_request(struct crypto_op* op) {
    printf("Entering process_crypto_request\n");
    if (op->type != CRYPTO_OP_TYPE_ENCRYPT && op->type != CRYPTO_OP_TYPE_DECRYPT) {
        printf("Invalid operation type: %d\n", op->type);
        op->state = CRYPTO_OP_STATE_ERROR;
        return;
    }

    uint8_t *key = op->payload;
    uint8_t *data = op->payload + op->key_size;
    for (uint64_t i = 0; i < op->data_size; i++) {
        data[i] ^= key[i % op->key_size];
    }
    // Memory barrier to ensure data is written before setting the state to DONE
    asm volatile ("fence rw,rw" : : : "memory");

    op->state = CRYPTO_OP_STATE_DONE;
    printf("Exiting process_crypto_request\n");
}

int main(void) {
    // Check if the process PID is 2
    if (getpid() != 2) {
        printf("crypto_srv: Invalid PID, exiting\n");
        exit(1);
    }

    // Open the console
    if(open("console", O_RDWR) < 0) {
        mknod("console", CONSOLE, 0);
        open("console", O_RDWR);
    }
    dup(0);  // stdout
    dup(0);  // stderr

    printf("crypto_srv: starting\n");

    while (1) {
        void *addr;
        uint64 size;

        // Take a shared memory request
        if (take_shared_memory_request(&addr, &size) == -1) {
            sleep(1); // Sleep briefly if no request is available
            continue;
        }

        // Ensure the request is valid
        struct crypto_op *op = (struct crypto_op*)addr;
        if (op->state != CRYPTO_OP_STATE_INIT || 
            (op->type != CRYPTO_OP_TYPE_ENCRYPT && op->type != CRYPTO_OP_TYPE_DECRYPT)) {
            printf("Invalid state or type. State: %d, Type: %d\n", op->state, op->type);
            op->state = CRYPTO_OP_STATE_ERROR;
            remove_shared_memory_request(addr, size);
            continue;
        }

        // Perform XOR operation for encryption/decryption
        process_crypto_request(op);

        // Set the operation state to DONE
        asm volatile ("fence rw,rw" : : : "memory");

        // Remove the shared memory request
        if (remove_shared_memory_request(addr, size) == -1) {
            printf("Failed to remove shared memory request\n");
        }
    }

    exit(0);
}
