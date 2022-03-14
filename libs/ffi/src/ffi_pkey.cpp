#include <nil/crypto3/ffi/ffi.h>
#include <nil/crypto3/ffi/ffi_util.h>
#include <nil/crypto3/ffi/ffi_pkey.h>
#include <nil/crypto3/ffi/ffi_rng.h>

#if defined(CRYPTO3_HAS_HASH_ID)
#include <nil/crypto3/pkpad/hash_id/hash_id.hpp>
#endif

extern "C" {

using namespace nil::crypto3::ffi;

int nil_crypto_privkey_create(nil_crypto_privkey_t *key_obj, const char *algo_name, const char *algo_params,
                              nil_crypto_rng_t rng_obj) {
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        if (key_obj == NULL) {
            return CRYPTO3_FFI_ERROR_NULL_POINTER;
        }

        *key_obj = NULL;
        if (rng_obj == NULL) {
            return CRYPTO3_FFI_ERROR_NULL_POINTER;
        }

        //        nil::crypto3::RandomNumberGenerator &rng = safe_get(rng_obj);
        //        std::unique_ptr<nil::crypto3::private_key> key(
        //            nil::crypto3::create_private_key(algo_name ? algo_name : "RSA", rng, algo_params ? algo_params :
        //            ""));

        //        if (key) {
        //            *key_obj = new nil_crypto_privkey_struct(key.release());
        //            return CRYPTO3_FFI_SUCCESS;
        //        } else {
        //            return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
        //        }
    });
}

int nil_crypto_privkey_load(nil_crypto_privkey_t *key, nil_crypto_rng_t rng_obj, const uint8_t bits[], size_t len,
                            const char *password) {
    *key = NULL;

    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        //        nil::crypto3::DataSource_Memory src(bits, len);

        //        std::unique_ptr<nil::crypto3::private_key> pkcs8;

        //        if (password == NULL) {
        //            pkcs8 = nil::crypto3::PKCS8::load_key(src);
        //        } else {
        //            pkcs8 = nil::crypto3::PKCS8::load_key(src, std::string(password));
        //        }
        //
        //        if (pkcs8) {
        //            *key = new nil_crypto_privkey_struct(pkcs8.release());
        //            return CRYPTO3_FFI_SUCCESS;
        //        }
        return CRYPTO3_FFI_ERROR_UNKNOWN_ERROR;
    });
}

int nil_crypto_privkey_destroy(nil_crypto_privkey_t key) {
    //    return CRYPTO3_FFI_CHECKED_DELETE(key);
}

int nil_crypto_pubkey_load(nil_crypto_pubkey_t *key, const uint8_t bits[], size_t bits_len) {
    *key = NULL;

    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        //        nil::crypto3::DataSource_Memory src(bits, bits_len);
        //        std::unique_ptr<nil::crypto3::Public_Key> pubkey(nil::crypto3::X509::load_key(src));
        //
        //        if (pubkey == NULL) {
        //            return CRYPTO3_FFI_ERROR_UNKNOWN_ERROR;
        //        }
        //
        //        *key = new nil_crypto_pubkey_struct(pubkey.release());
        return CRYPTO3_FFI_SUCCESS;
    });
}

int nil_crypto_pubkey_destroy(nil_crypto_pubkey_t key) {
    //    return CRYPTO3_FFI_CHECKED_DELETE(key);
}

int nil_crypto_privkey_export_pubkey(nil_crypto_pubkey_t *pubout, nil_crypto_privkey_t key_obj) {
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        //        std::unique_ptr<nil::crypto3::Public_Key> pubkey(
        //            nil::crypto3::X509::load_key(nil::crypto3::X509::ber_encode(safe_get(key_obj))));
        //
        //        *pubout = new nil_crypto_pubkey_struct(pubkey.release());
        return CRYPTO3_FFI_SUCCESS;
    });
}

int nil_crypto_privkey_algo_name(nil_crypto_privkey_t key, char out[], size_t *out_len) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::private_key, key, k, { return write_str_output(out, out_len,
    //    k.algo_name()); });
}

int nil_crypto_pubkey_algo_name(nil_crypto_pubkey_t key, char out[], size_t *out_len) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::Public_Key, key, k, { return write_str_output(out, out_len,
    //    k.algo_name()); });
}

int nil_crypto_pubkey_check_key(nil_crypto_pubkey_t key, nil_crypto_rng_t rng, uint32_t flags) {
    const bool strong = (flags & CRYPTO3_CHECK_KEY_EXPENSIVE_TESTS);

    //    return CRYPTO3_FFI_DO(nil::crypto3::Public_Key, key, k,
    //                          { return (k.check_key(safe_get(rng), strong) == true) ? 0 : -1; });
}

int nil_crypto_privkey_check_key(nil_crypto_privkey_t key, nil_crypto_rng_t rng, uint32_t flags) {
    const bool strong = (flags & CRYPTO3_CHECK_KEY_EXPENSIVE_TESTS);
    //    return CRYPTO3_FFI_DO(nil::crypto3::private_key, key, k,
    //                          { return (k.check_key(safe_get(rng), strong) == true) ? 0 : -1; });
}

int nil_crypto_pubkey_export(nil_crypto_pubkey_t key, uint8_t out[], size_t *out_len, uint32_t flags) {
//    return CRYPTO3_FFI_DO(nil::crypto3::Public_Key, key, k, {
        //        if (flags == CRYPTO3_PRIVKEY_EXPORT_FLAG_DER)
        //            return write_vec_output(out, out_len, nil::crypto3::X509::ber_encode(k));
        //        else if (flags == CRYPTO3_PRIVKEY_EXPORT_FLAG_PEM)
        //            return write_str_output(out, out_len, nil::crypto3::X509::pem_encode(k));
        //        else
        return CRYPTO3_FFI_ERROR_BAD_FLAG;
//    });
}

int nil_crypto_privkey_export(nil_crypto_privkey_t key, uint8_t out[], size_t *out_len, uint32_t flags) {
//    return CRYPTO3_FFI_DO(nil::crypto3::private_key, key, k, {
        //        if (flags == CRYPTO3_PRIVKEY_EXPORT_FLAG_DER)
        //            return write_vec_output(out, out_len, nil::crypto3::PKCS8::ber_encode(k));
        //        else if (flags == CRYPTO3_PRIVKEY_EXPORT_FLAG_PEM)
        //            return write_str_output(out, out_len, nil::crypto3::PKCS8::pem_encode(k));
        //        else
        return CRYPTO3_FFI_ERROR_BAD_FLAG;
//    });
}

int nil_crypto_privkey_export_encrypted(nil_crypto_privkey_t key, uint8_t out[], size_t *out_len,
                                        nil_crypto_rng_t rng_obj, const char *pass, const char * /*ignored - pbe*/,
                                        uint32_t flags) {
    return nil_crypto_privkey_export_encrypted_pbkdf_iter(key, out, out_len, rng_obj, pass, 100000, NULL, NULL, flags);
}

int nil_crypto_privkey_export_encrypted_pbkdf_msec(nil_crypto_privkey_t key, uint8_t out[], size_t *out_len,
                                                   nil_crypto_rng_t rng_obj, const char *pass, uint32_t pbkdf_msec,
                                                   size_t *pbkdf_iters_out, const char *maybe_cipher,
                                                   const char *maybe_pbkdf_hash, uint32_t flags) {
//    return CRYPTO3_FFI_DO(nil::crypto3::private_key, key, k, {
        const std::chrono::milliseconds pbkdf_time(pbkdf_msec);
        //        nil::crypto3::RandomNumberGenerator &rng = safe_get(rng_obj);

        const std::string cipher = (maybe_cipher ? maybe_cipher : "");
        const std::string pbkdf_hash = (maybe_pbkdf_hash ? maybe_pbkdf_hash : "");

        //        if (flags == CRYPTO3_PRIVKEY_EXPORT_FLAG_DER) {
        //            return write_vec_output(out, out_len,
        //                                    nil::crypto3::PKCS8::ber_encode_encrypted_pbkdf_msec(
        //                                        k, rng, pass, pbkdf_time, pbkdf_iters_out, cipher, pbkdf_hash));
        //        } else if (flags == CRYPTO3_PRIVKEY_EXPORT_FLAG_PEM) {
        //            return write_str_output(out, out_len,
        //                                    nil::crypto3::PKCS8::pem_encode_encrypted_pbkdf_msec(
        //                                        k, rng, pass, pbkdf_time, pbkdf_iters_out, cipher, pbkdf_hash));
        //        } else {
        return -2;
        //        }
//    });
}

int nil_crypto_privkey_export_encrypted_pbkdf_iter(nil_crypto_privkey_t key, uint8_t out[], size_t *out_len,
                                                   nil_crypto_rng_t rng_obj, const char *pass, size_t pbkdf_iter,
                                                   const char *maybe_cipher, const char *maybe_pbkdf_hash,
                                                   uint32_t flags) {
//    return CRYPTO3_FFI_DO(nil::crypto3::private_key, key, k, {
//        nil::crypto3::RandomNumberGenerator &rng = safe_get(rng_obj);

        const std::string cipher = (maybe_cipher ? maybe_cipher : "");
        const std::string pbkdf_hash = (maybe_pbkdf_hash ? maybe_pbkdf_hash : "");

//        if (flags == CRYPTO3_PRIVKEY_EXPORT_FLAG_DER) {
//            return write_vec_output(
//                out, out_len,
//                nil::crypto3::PKCS8::ber_encode_encrypted_pbkdf_iter(k, rng, pass, pbkdf_iter, cipher, pbkdf_hash));
//        } else if (flags == CRYPTO3_PRIVKEY_EXPORT_FLAG_PEM) {
//            return write_str_output(
//                out, out_len,
//                nil::crypto3::PKCS8::pem_encode_encrypted_pbkdf_iter(k, rng, pass, pbkdf_iter, cipher, pbkdf_hash));
//        } else {
            return -2;
//        }
//    });
}

int nil_crypto_pubkey_estimated_strength(nil_crypto_pubkey_t key, size_t *estimate) {
//    return CRYPTO3_FFI_DO(nil::crypto3::Public_Key, key, k, { *estimate = k.estimated_strength(); });
}

int nil_crypto_pubkey_fingerprint(nil_crypto_pubkey_t key, const char *hash_fn, uint8_t out[], size_t *out_len) {
//    return CRYPTO3_FFI_DO(nil::crypto3::Public_Key, key, k, {
//        std::unique_ptr<nil::crypto3::HashFunction> h(nil::crypto3::HashFunction::create(hash_fn));
//        return write_vec_output(out, out_len, h->process(k.public_key_bits()));
//    });
}

int nil_crypto_pkcs_hash_id(const char *hash_name, uint8_t pkcs_id[], size_t *pkcs_id_len) {
#if defined(CRYPTO3_HAS_HASH_ID)
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        const std::vector<uint8_t> hash_id = nil::crypto3::pkcs_hash_id(hash_name);
        return write_output(pkcs_id, pkcs_id_len, hash_id.data(), hash_id.size());
    });
#else
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}
}