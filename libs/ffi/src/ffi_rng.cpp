#include <nil/crypto3/ffi/ffi.h>
#include <nil/crypto3/ffi/ffi_util.h>
#include <nil/crypto3/ffi/ffi_rng.h>

extern "C" {

using namespace nil::crypto3::ffi;

int nil_crypto_rng_init(nil_crypto_rng_t *rng_out, const char *rng_type) {
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        if (rng_out == NULL) {
            return CRYPTO3_FFI_ERROR_NULL_POINTER;
        }

        const std::string rng_type_s(rng_type ? rng_type : "system");

        //        std::unique_ptr<nil::crypto3::RandomNumberGenerator> rng;
        //
        //        if (rng_type_s == "system") {
        //            rng.reset(new nil::crypto3::System_RNG);
        //        } else if (rng_type_s == "user") {
        //            rng.reset(new nil::crypto3::AutoSeeded_RNG);
        //        } else {
        //            return CRYPTO3_FFI_ERROR_BAD_PARAMETER;
        //        }
        //
        //        *rng_out = new nil_crypto_rng_struct(rng.release());
        return CRYPTO3_FFI_SUCCESS;
    });
}

int nil_crypto_rng_destroy(nil_crypto_rng_t rng) {
    //    return CRYPTO3_FFI_CHECKED_DELETE(rng);
}

int nil_crypto_rng_get(nil_crypto_rng_t rng, uint8_t *out, size_t out_len) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::RandomNumberGenerator, rng, r, { r.randomize(out, out_len); });
}

int nil_crypto_rng_reseed(nil_crypto_rng_t rng, size_t bits) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::RandomNumberGenerator, rng, r,
    //                          { r.reseed_from_rng(nil::crypto3::system_rng(), bits); });
}
}
