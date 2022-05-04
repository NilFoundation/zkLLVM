#include <nil/crypto3/ffi/ffi.h>
#include <nil/crypto3/ffi/ffi_util.h>

#include <boost/current_function.hpp>

#include <nil/crypto3/hash/hash_state.hpp>

extern "C" {

using namespace nil::crypto3::ffi;

// CRYPTO3_FFI_DECLARE_STRUCT(nil_crypto_hash_struct, nil::crypto3::accumulator_set, 0x1F0A4F84);

int nil_crypto_hash_init(nil_crypto_hash_t *hash, const char *hash_name, uint32_t flags) {
    return ffi_guard_thunk(BOOST_CURRENT_FUNCTION, [=]() -> int {
        if (hash == NULL || hash_name == NULL || *hash_name == 0) {
            return CRYPTO3_FFI_ERROR_NULL_POINTER;
        }
        if (flags != 0) {
            return CRYPTO3_FFI_ERROR_BAD_FLAG;
        }

        //        std::unique_ptr<nil::crypto3::HashFunction> h = nil::crypto3::HashFunction::create(hash_name);
        //        if (h == NULL) {
        //            return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
        //        }

        //        *hash = new nil_crypto_hash_struct(h.release());
        return CRYPTO3_FFI_SUCCESS;
    });
}

int nil_crypto_hash_destroy(nil_crypto_hash_t hash) {
    //    return CRYPTO3_FFI_CHECKED_DELETE(hash);
}

int nil_crypto_hash_output_length(nil_crypto_hash_t hash, size_t *out) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::HashFunction, hash, h, { *out = h.output_length(); });
}

int nil_crypto_hash_block_size(nil_crypto_hash_t hash, size_t *out) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::HashFunction, hash, h, { *out = h.hash_block_size(); });
}

int nil_crypto_hash_clear(nil_crypto_hash_t hash) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::HashFunction, hash, h, { h.clear(); });
}

int nil_crypto_hash_update(nil_crypto_hash_t hash, const uint8_t *buf, size_t len) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::HashFunction, hash, h, { h.update(buf, len); });
}

int nil_crypto_hash_final(nil_crypto_hash_t hash, uint8_t out[]) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::HashFunction, hash, h, { h.final(out); });
}

int nil_crypto_hash_copy_state(nil_crypto_hash_t *dest, const nil_crypto_hash_t source) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::HashFunction, source, src,
    //                          { *dest = new nil_crypto_hash_struct(src.copy_state().release()); });
}
}
