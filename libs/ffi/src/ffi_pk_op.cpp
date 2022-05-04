#include <nil/crypto3/ffi/ffi.h>
#include <nil/crypto3/ffi/ffi_util.h>
#include <nil/crypto3/ffi/ffi_pkey.h>
#include <nil/crypto3/ffi/ffi_rng.h>

#include <boost/assert.hpp>
#include <boost/current_function.hpp>

extern "C" {

using namespace nil::crypto3::ffi;

// CRYPTO3_FFI_DECLARE_STRUCT(nil_crypto_pk_op_encrypt_struct, nil::crypto3::PK_Encryptor, 0x891F3FC3);
// CRYPTO3_FFI_DECLARE_STRUCT(nil_crypto_pk_op_decrypt_struct, nil::crypto3::PK_Decryptor, 0x912F3C37);
// CRYPTO3_FFI_DECLARE_STRUCT(nil_crypto_pk_op_sign_struct, nil::crypto3::PK_Signer, 0x1AF0C39F);
// CRYPTO3_FFI_DECLARE_STRUCT(nil_crypto_pk_op_verify_struct, nil::crypto3::PK_Verifier, 0x2B91F936);
// CRYPTO3_FFI_DECLARE_STRUCT(nil_crypto_pk_op_ka_struct, nil::crypto3::pk_key_agreement, 0x2939CAB1);

int nil_crypto_pk_op_encrypt_create(nil_crypto_pk_op_encrypt_t *op, nil_crypto_pubkey_t key_obj, const char *padding,
                                    uint32_t flags) {
    return ffi_guard_thunk(BOOST_CURRENT_FUNCTION, [=]() -> int {
        BOOST_ASSERT(op != NULL);

        *op = NULL;

        if (flags != 0) {
            return CRYPTO3_FFI_ERROR_BAD_FLAG;
        }

        //        std::unique_ptr<nil::crypto3::PK_Encryptor> pk(
        //            new nil::crypto3::PK_Encryptor_EME(safe_get(key_obj), nil::crypto3::system_rng(), padding));
        //        *op = new nil_crypto_pk_op_encrypt_struct(pk.release());
        return CRYPTO3_FFI_SUCCESS;
    });
}

int nil_crypto_pk_op_encrypt_destroy(nil_crypto_pk_op_encrypt_t op) {
    //    return CRYPTO3_FFI_CHECKED_DELETE(op);
}

int nil_crypto_pk_op_encrypt(nil_crypto_pk_op_encrypt_t op, nil_crypto_rng_t rng_obj, uint8_t out[], size_t *out_len,
                             const uint8_t plaintext[], size_t plaintext_len) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::PK_Encryptor, op, o, {
    //        return write_vec_output(out, out_len, o.encrypt(plaintext, plaintext_len, safe_get(rng_obj)));
    //    });
}

/*
 * Public Key Decryption
 */
int nil_crypto_pk_op_decrypt_create(nil_crypto_pk_op_decrypt_t *op, nil_crypto_privkey_t key_obj, const char *padding,
                                    uint32_t flags) {
    return ffi_guard_thunk(BOOST_CURRENT_FUNCTION, [=]() -> int {
        BOOST_ASSERT(op != NULL);

        *op = NULL;

        if (flags != 0) {
            return CRYPTO3_FFI_ERROR_BAD_FLAG;
        }

        //        std::unique_ptr<nil::crypto3::PK_Decryptor> pk(
        //            new nil::crypto3::PK_Decryptor_EME(safe_get(key_obj), nil::crypto3::system_rng(), padding));
        //        *op = new nil_crypto_pk_op_decrypt_struct(pk.release());
        return CRYPTO3_FFI_SUCCESS;
    });
}

int nil_crypto_pk_op_decrypt_destroy(nil_crypto_pk_op_decrypt_t op) {
//    return CRYPTO3_FFI_CHECKED_DELETE(op);
}

int nil_crypto_pk_op_decrypt(nil_crypto_pk_op_decrypt_t op, uint8_t out[], size_t *out_len, const uint8_t ciphertext[],
                             size_t ciphertext_len) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::PK_Decryptor, op, o,
    //                          { return write_vec_output(out, out_len, o.decrypt(ciphertext, ciphertext_len)); });
}

/*
 * Signature Generation
 */
int nil_crypto_pk_op_sign_create(nil_crypto_pk_op_sign_t *op, nil_crypto_privkey_t key_obj, const char *hash,
                                 uint32_t flags) {
    return ffi_guard_thunk(BOOST_CURRENT_FUNCTION, [=]() -> int {
        BOOST_ASSERT(op != NULL);

        *op = NULL;

        if (flags != 0) {
            return CRYPTO3_FFI_ERROR_BAD_FLAG;
        }

        //        std::unique_ptr<nil::crypto3::PK_Signer> pk(
        //            new nil::crypto3::PK_Signer(safe_get(key_obj), nil::crypto3::system_rng(), hash));
        //        *op = new nil_crypto_pk_op_sign_struct(pk.release());
        return CRYPTO3_FFI_SUCCESS;
    });
}

int nil_crypto_pk_op_sign_destroy(nil_crypto_pk_op_sign_t op) {
//    return CRYPTO3_FFI_CHECKED_DELETE(op);
}

int nil_crypto_pk_op_sign_update(nil_crypto_pk_op_sign_t op, const uint8_t in[], size_t in_len) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::PK_Signer, op, o, { o.update(in, in_len); });
}

int nil_crypto_pk_op_sign_finish(nil_crypto_pk_op_sign_t op, nil_crypto_rng_t rng_obj, uint8_t out[], size_t *out_len) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::PK_Signer, op, o,
    //                          { return write_vec_output(out, out_len, o.signature(safe_get(rng_obj))); });
}

int nil_crypto_pk_op_verify_create(nil_crypto_pk_op_verify_t *op, nil_crypto_pubkey_t key_obj, const char *hash,
                                   uint32_t flags) {
    return ffi_guard_thunk(BOOST_CURRENT_FUNCTION, [=]() -> int {
        BOOST_ASSERT(op != NULL);

        if (flags != 0) {
            return CRYPTO3_FFI_ERROR_BAD_FLAG;
        }

        //        std::unique_ptr<nil::crypto3::PK_Verifier> pk(new nil::crypto3::PK_Verifier(safe_get(key_obj), hash));
        //        *op = new nil_crypto_pk_op_verify_struct(pk.release());
        return CRYPTO3_FFI_SUCCESS;
    });
}

int nil_crypto_pk_op_verify_destroy(nil_crypto_pk_op_verify_t op) {
//    return CRYPTO3_FFI_CHECKED_DELETE(op);
}

int nil_crypto_pk_op_verify_update(nil_crypto_pk_op_verify_t op, const uint8_t in[], size_t in_len) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::PK_Verifier, op, o, { o.update(in, in_len); });
}

int nil_crypto_pk_op_verify_finish(nil_crypto_pk_op_verify_t op, const uint8_t sig[], size_t sig_len) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::PK_Verifier, op, o, {
    //        const bool legit = o.check_signature(sig, sig_len);
    //
    //        if (legit)
    //            return CRYPTO3_FFI_SUCCESS;
    //        else
    //            return CRYPTO3_FFI_INVALID_VERIFIER;
    //    });
}

int nil_crypto_pk_op_key_agreement_create(nil_crypto_pk_op_ka_t *op, nil_crypto_privkey_t key_obj, const char *kdf,
                                          uint32_t flags) {
    return ffi_guard_thunk(BOOST_CURRENT_FUNCTION, [=]() -> int {
        BOOST_ASSERT(op != NULL);

        *op = NULL;

        if (flags != 0) {
            return CRYPTO3_FFI_ERROR_BAD_FLAG;
        }

        //        std::unique_ptr<nil::crypto3::pk_key_agreement> pk(
        //            new nil::crypto3::pk_key_agreement(safe_get(key_obj), nil::crypto3::system_rng(), kdf));
        //        *op = new nil_crypto_pk_op_ka_struct(pk.release());
        return CRYPTO3_FFI_SUCCESS;
    });
}

int nil_crypto_pk_op_key_agreement_destroy(nil_crypto_pk_op_ka_t op) {
//    return CRYPTO3_FFI_CHECKED_DELETE(op);
}

int nil_crypto_pk_op_key_agreement_export_public(nil_crypto_privkey_t key, uint8_t out[], size_t *out_len) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::private_key, key, k, {
    //        if (auto kak = dynamic_cast<const nil::crypto3::pk_key_agreement_key *>(&k))
    //            return write_vec_output(out, out_len, kak->public_value());
    //        return CRYPTO3_FFI_ERROR_BAD_FLAG;
    //    });
}

int nil_crypto_pk_op_key_agreement(nil_crypto_pk_op_ka_t op, uint8_t out[], size_t *out_len, const uint8_t other_key[],
                                   size_t other_key_len, const uint8_t salt[], size_t salt_len) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::pk_key_agreement, op, o, {
    //        auto k = o.derive_key(*out_len, other_key, other_key_len, salt, salt_len).bits_of();
    //        return write_vec_output(out, out_len, k);
    //    });
}
}
