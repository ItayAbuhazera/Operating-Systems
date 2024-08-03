#include "kernel/types.h"
#include "kernel/crypto.h"
#include "user/user.h"

static const char message[] = {
  0x1b, 0x3b, 0x5b, 0x43, 0x12, 0x5d, 0x3c, 0x73, 0x53, 0x10, 0x41, 0x51,
  0x2c, 0x21, 0x57, 0x44, 0x12, 0x47, 0x3b, 0x21, 0x5b, 0x5e, 0x55, 0x14,
  0x3b, 0x3c, 0x12, 0x52, 0x57, 0x14, 0x2b, 0x36, 0x51, 0x42, 0x4b, 0x44,
  0x3b, 0x36, 0x56, 0x10, 0x50, 0x4d, 0x6f, 0x27, 0x5a, 0x55, 0x12, 0x57,
  0x3d, 0x2a, 0x42, 0x44, 0x5d, 0x14, 0x3c, 0x36, 0x40, 0x46, 0x57, 0x46
};

static const char key[] = "OS2024";

int main (void) {
  printf("crypto_cli: attempting to decrypt message\n");

  uint key_size = sizeof(key) - 1;
  uint data_size = sizeof(message);

  const uint op_size = sizeof(struct crypto_op) + key_size + data_size; // The size of the crypto_op struct plus the key and data
  struct crypto_op* op = malloc(op_size);  // Allocate memory for the crypto_op struct

  op->type = CRYPTO_OP_TYPE_DECRYPT;
  op->state = CRYPTO_OP_STATE_INIT;
  op->key_size = key_size;
  op->data_size = data_size;

  memcpy(op->payload, key, key_size);
  memcpy(op->payload + key_size, message, data_size);

  crypto_op(op, op_size);
  
  volatile enum crypto_op_state* op_state = &op->state;
  while (*op_state == CRYPTO_OP_STATE_INIT)
    ;

  // Sleep just to avoid printing the message before the server
  sleep(1);

  if (*op_state == CRYPTO_OP_STATE_ERROR) {
    printf("crypto_cli: crypto operation failed\n");
    exit(1);
  } else {
    printf("crypto_cli: decrypted message: %s\n", op->payload + key_size);
  }

  exit (0);
}
