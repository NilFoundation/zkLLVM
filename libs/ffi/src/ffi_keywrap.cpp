#include <nil/crypto3/ffi/ffi.h>
#include <nil/crypto3/ffi/ffi_util.h>

#if defined(CRYPTO3_HAS_RFC3394_KEYWRAP)
#include <nil/crypto3/misc/rfc3394/rfc3394.hpp>
#endif

extern "C" {

using namespace nil::crypto3::ffi;

int nil_crypto_key_wrap3394(const uint8_t key[], size_t key_len, const uint8_t kek[], size_t kek_len,
                            uint8_t wrapped_key[], size_t *wrapped_key_len) {
#if defined(CRYPTO3_HAS_RFC3394_KEYWRAP)
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        const nil::crypto3::symmetric_key kek_sym(kek, kek_len);
        const nil::crypto3::secure_vector<uint8_t> key_pt(key, key + key_len);
        const nil::crypto3::secure_vector<uint8_t> key_ct = nil::crypto3::rfc3394_keywrap(key_pt, kek_sym);
        return write_vec_output(wrapped_key, wrapped_key_len, key_ct);
    });
#else
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}

int nil_crypto_key_unwrap3394(const uint8_t wrapped_key[], size_t wrapped_key_len, const uint8_t kek[], size_t kek_len,
                              uint8_t key[], size_t *key_len) {
#if defined(CRYPTO3_HAS_RFC3394_KEYWRAP)
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        const nil::crypto3::symmetric_key kek_sym(kek, kek_len);
        const nil::crypto3::secure_vector<uint8_t> key_ct(wrapped_key, wrapped_key + wrapped_key_len);
        const nil::crypto3::secure_vector<uint8_t> key_pt = nil::crypto3::rfc3394_keyunwrap(key_ct, kek_sym);
        return write_vec_output(key, key_len, key_pt);
    });
#else
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}
}
