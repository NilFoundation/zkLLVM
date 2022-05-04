#include <nil/crypto3/ffi/ffi.h>
#include <nil/crypto3/ffi/ffi_util.h>

#include <boost/current_function.hpp>

extern "C" {

using namespace nil::crypto3::ffi;

// CRYPTO3_FFI_DECLARE_STRUCT(nil_crypto_mac_struct, nil::crypto3::MessageAuthenticationCode, 0xA06E8FC1);

int nil_crypto_mac_init(nil_crypto_mac_t *mac, const char *mac_name, uint32_t flags) {
    return ffi_guard_thunk(BOOST_CURRENT_FUNCTION, [=]() -> int {
        if (!mac || !mac_name || flags != 0) {
            return CRYPTO3_FFI_ERROR_NULL_POINTER;
        }

        //        std::unique_ptr<nil::crypto3::MessageAuthenticationCode> m =
        //            nil::crypto3::MessageAuthenticationCode::create(mac_name);

        //        if (m == NULL) {
        //            return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
        //        }

        //        *mac = new nil_crypto_mac_struct(m.release());
        return CRYPTO3_FFI_SUCCESS;
    });
}

int nil_crypto_mac_destroy(nil_crypto_mac_t mac) {
    //    return CRYPTO3_FFI_CHECKED_DELETE(mac);
}

int nil_crypto_mac_set_key(nil_crypto_mac_t mac, const uint8_t *key, size_t key_len) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::MessageAuthenticationCode, mac, m, { m.set_key(key, key_len); });
}

int nil_crypto_mac_output_length(nil_crypto_mac_t mac, size_t *out) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::MessageAuthenticationCode, mac, m, { *out = m.output_length(); });
}

int nil_crypto_mac_clear(nil_crypto_mac_t mac) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::MessageAuthenticationCode, mac, m, { m.clear(); });
}

int nil_crypto_mac_update(nil_crypto_mac_t mac, const uint8_t *buf, size_t len) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::MessageAuthenticationCode, mac, m, { m.update(buf, len); });
}

int nil_crypto_mac_final(nil_crypto_mac_t mac, uint8_t out[]) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::MessageAuthenticationCode, mac, m, { m.final(out); });
}
}
