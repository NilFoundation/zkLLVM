#include <nil/crypto3/ffi/ffi.h>
#include <nil/crypto3/ffi/ffi_util.h>

extern "C" {

using namespace nil::crypto3::ffi;

// CRYPTO3_FFI_DECLARE_STRUCT(nil_crypto_block_cipher_struct, nil::crypto3::BlockCipher, 0x64C29716);

int nil_crypto_block_cipher_init(nil_crypto_block_cipher_t *bc, const char *bc_name) {
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        if (bc == NULL || bc_name == NULL || *bc_name == 0) {
            return CRYPTO3_FFI_ERROR_NULL_POINTER;
        }

        *bc = NULL;

        //        std::unique_ptr<nil::crypto3::old_block_cipher> cipher(nil::crypto3::BlockCipher::create(bc_name));
        //        if (cipher == NULL) {
        //            return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
        //        }
        //
        //        *bc = new nil_crypto_block_cipher_struct(cipher.release());
        return CRYPTO3_FFI_SUCCESS;
    });
}

/**
 * Destroy a block cipher object
 */
int nil_crypto_block_cipher_destroy(nil_crypto_block_cipher_t bc) {
    //    return CRYPTO3_FFI_CHECKED_DELETE(bc);
}

int nil_crypto_block_cipher_clear(nil_crypto_block_cipher_t bc) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::BlockCipher, bc, b, {
    //        b.clear();
    //    });
}

/**
 * Set the key for a block cipher instance
 */
int nil_crypto_block_cipher_set_key(nil_crypto_block_cipher_t bc, const uint8_t key[], size_t len) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::BlockCipher, bc, b, {
    //        b.set_key(key, len);
    //    });
}

/**
 * Return the positive block size of this block cipher, or negative to
 * indicate an error
 */
int nil_crypto_block_cipher_block_size(nil_crypto_block_cipher_t bc) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::BlockCipher, bc, b, {
    //        return static_cast<int>(b.block_size());
    //    });
}

int nil_crypto_block_cipher_encrypt_blocks(nil_crypto_block_cipher_t bc, const uint8_t in[], uint8_t out[],
                                           size_t blocks) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::BlockCipher, bc, b, {
    //        b.encrypt_n(in, out, blocks);
    //    });
}

int nil_crypto_block_cipher_decrypt_blocks(nil_crypto_block_cipher_t bc, const uint8_t in[], uint8_t out[],
                                           size_t blocks) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::BlockCipher, bc, b, {
    //        b.decrypt_n(in, out, blocks);
    //    });
}
}
