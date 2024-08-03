enum crypto_op_type {
    CRYPTO_OP_TYPE_ENCRYPT = 1,
    CRYPTO_OP_TYPE_DECRYPT,
};

enum crypto_op_state {
    CRYPTO_OP_STATE_INIT = 1,
    CRYPTO_OP_STATE_DONE,
    CRYPTO_OP_STATE_ERROR
};

struct crypto_op {
    enum crypto_op_type type;
    enum crypto_op_state state;

    uint64 key_size;
    uint64 data_size;

    uchar payload[];
};