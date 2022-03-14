#include <nil/crypto3/ffi/ffi.h>
#include <nil/crypto3/ffi/ffi_util.h>
#include <nil/crypto3/ffi/ffi_rng.h>

#if defined(CRYPTO3_HAS_BCRYPT)
#include <nil/crypto3/passhash/bcrypt/bcrypt.hpp>
#endif

extern "C" {

using namespace nil::crypto3::ffi;

int nil_crypto_pbkdf(const char *pbkdf_algo, uint8_t out[], size_t out_len, const char *pass, const uint8_t salt[],
                     size_t salt_len, size_t iterations) {
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        //        std::unique_ptr<nil::crypto3::PBKDF> pbkdf(nil::crypto3::get_pbkdf(pbkdf_algo));
        //        pbkdf->pbkdf_iterations(out, out_len, pass, salt, salt_len, iterations);
        return CRYPTO3_FFI_SUCCESS;
    });
}

int nil_crypto_pbkdf_timed(const char *pbkdf_algo, uint8_t out[], size_t out_len, const char *password,
                           const uint8_t salt[], size_t salt_len, size_t ms_to_run, size_t *iterations_used) {
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        //        std::unique_ptr<nil::crypto3::PBKDF> pbkdf(nil::crypto3::get_pbkdf(pbkdf_algo));
        //        pbkdf->pbkdf_timed(out, out_len, password, salt, salt_len, std::chrono::milliseconds(ms_to_run),
        //                           *iterations_used);
        return CRYPTO3_FFI_SUCCESS;
    });
}

int nil_crypto_kdf(const char *kdf_algo, uint8_t out[], size_t out_len, const uint8_t secret[], size_t secret_len,
                   const uint8_t salt[], size_t salt_len, const uint8_t label[], size_t label_len) {
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        //        std::unique_ptr<nil::crypto3::KDF> kdf(nil::crypto3::get_kdf(kdf_algo));
        //        kdf->kdf(out, out_len, secret, secret_len, salt, salt_len, label, label_len);
        return CRYPTO3_FFI_SUCCESS;
    });
}

int nil_crypto_bcrypt_generate(uint8_t *out, size_t *out_len, const char *pass, nil_crypto_rng_t rng_obj, size_t wf,
                               uint32_t flags) {
#if defined(CRYPTO3_HAS_BCRYPT)
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        if (out == NULL || out_len == NULL || pass == NULL)
            return CRYPTO3_FFI_ERROR_NULL_POINTER;

        if (flags != 0)
            return CRYPTO3_FFI_ERROR_BAD_FLAG;

        if (wf < 4 || wf > 18)
            throw FFI_Error("Bad bcrypt work factor " + std::to_string(wf));

        nil::crypto3::RandomNumberGenerator &rng = safe_get(rng_obj);
        const std::string bcrypt = nil::crypto3::generate_bcrypt(pass, rng, static_cast<uint16_t>(wf));
        return write_str_output(out, out_len, bcrypt);
    });
#else
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}

int nil_crypto_bcrypt_is_valid(const char *pass, const char *hash) {
#if defined(CRYPTO3_HAS_BCRYPT)
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        return nil::crypto3::check_bcrypt(pass, hash) ? CRYPTO3_FFI_SUCCESS : CRYPTO3_FFI_INVALID_VERIFIER;
    });
#else
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}
}
