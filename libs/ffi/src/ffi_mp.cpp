#include <nil/crypto3/ffi/ffi.h>
#include <nil/crypto3/ffi/ffi_util.h>
#include <nil/crypto3/ffi/ffi_rng.h>
#include <nil/crypto3/ffi/ffi_mp.h>

extern "C" {

using namespace nil::crypto3::ffi;

int nil_crypto_mp_init(nil_crypto_mp_t *mp_out) {
    return ffi_guard_thunk(BOOST_CURRENT_FUNCTION, [=]() -> int {
        if (mp_out == NULL) {
            return CRYPTO3_FFI_ERROR_NULL_POINTER;
        }

        //        *mp_out = new nil_crypto_mp_struct(new nil::crypto3::multiprecision::cpp_int);
        return CRYPTO3_FFI_SUCCESS;
    });
}

int nil_crypto_mp_clear(nil_crypto_mp_t mp) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, mp, bn, { bn.clear(); });
}

int nil_crypto_mp_set_from_int(nil_crypto_mp_t mp, int initial_value) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, mp, bn, {
    //        if (initial_value >= 0) {
    //            bn = nil::crypto3::multiprecision::cpp_int(static_cast<uint64_t>(initial_value));
    //        } else {
    //            bn = nil::crypto3::multiprecision::cpp_int(static_cast<uint64_t>(-initial_value));
    //            bn.flip_sign();
    //        }
    //    });
}

int nil_crypto_mp_set_from_str(nil_crypto_mp_t mp, const char *str) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, mp, bn, { bn =
    //    nil::crypto3::multiprecision::cpp_int(str); });
}

int nil_crypto_mp_set_from_radix_str(nil_crypto_mp_t mp, const char *str, size_t radix) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, mp, bn, {
    //        nil::crypto3::multiprecision::cpp_int::Base base;
    //        if (radix == 10)
    //            base = nil::crypto3::multiprecision::cpp_int::Decimal;
    //        else if (radix == 16)
    //            base = nil::crypto3::multiprecision::cpp_int::Hexadecimal;
    //        else
    //            return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
    //
    //        const uint8_t *bytes = nil::crypto3::cast_char_ptr_to_uint8(str);
    //        const size_t len = strlen(str);
    //
    //        bn = nil::crypto3::multiprecision::cpp_int(bytes, len, base);
    //    });
}

int nil_crypto_mp_set_from_mp(nil_crypto_mp_t dest, const nil_crypto_mp_t source) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, dest, bn, { bn = safe_get(source); });
}

int nil_crypto_mp_is_negative(const nil_crypto_mp_t mp) {
    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, mp, bn, { return bn < 0 ? 1 : 0; });
}

int nil_crypto_mp_is_positive(const nil_crypto_mp_t mp) {
    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, mp, bn, { return bn > 0 ? 1 : 0; });
}

int nil_crypto_mp_flip_sign(nil_crypto_mp_t mp) {
    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, mp, bn, { -bn; });
}

int nil_crypto_mp_from_bin(nil_crypto_mp_t mp, const uint8_t bin[], size_t bin_len) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, mp, bn, { bn.binary_decode(bin, bin_len); });
}

int nil_crypto_mp_to_hex(const nil_crypto_mp_t mp, char *out) {
    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, mp, bn,
                          {
                              //        const std::string hex = bn.to_hex_string();
                              //        std::strcpy(out, hex.c_str());
                          });
}

int nil_crypto_mp_to_str(const nil_crypto_mp_t mp, uint8_t digit_base, char *out, size_t *out_len) {
    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, mp, bn, {
        //        if (digit_base == 0 || digit_base == 10)
        //            return write_str_output(out, out_len, bn.to_dec_string());
        //        else if (digit_base == 16)
        //            return write_str_output(out, out_len, bn.to_hex_string());
        //        else
        return CRYPTO3_FFI_ERROR_BAD_PARAMETER;
    });
}

int nil_crypto_mp_to_bin(const nil_crypto_mp_t mp, uint8_t vec[]) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, mp, bn, { bn.binary_encode(vec); });
}

int nil_crypto_mp_to_uint32(const nil_crypto_mp_t mp, uint32_t *val) {
    if (val == NULL) {
        return CRYPTO3_FFI_ERROR_NULL_POINTER;
    }
    //    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, mp, bn, { *val = bn.to_u32bit(); });
}

int nil_crypto_mp_destroy(nil_crypto_mp_t mp) {
    //    return CRYPTO3_FFI_CHECKED_DELETE(mp);
}

int nil_crypto_mp_add(nil_crypto_mp_t result, const nil_crypto_mp_t x, const nil_crypto_mp_t y) {
    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, result, res,
                          {
                              //        if (result == x)
                              //            res += safe_get(y);
                              //        else
                              //            res = safe_get(x) + safe_get(y);
                          });
}

int nil_crypto_mp_sub(nil_crypto_mp_t result, const nil_crypto_mp_t x, const nil_crypto_mp_t y) {
    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, result, res,
                          {
                              //        if (result == x)
                              //            res -= safe_get(y);
                              //        else
                              //            res = safe_get(x) - safe_get(y);
                          });
}

int nil_crypto_mp_add_u32(nil_crypto_mp_t result, const nil_crypto_mp_t x, uint32_t y) {
    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, result, res,
                          {
                              //        if (result == x)
                              //            res += static_cast<nil::crypto3::word>(y);
                              //        else
                              //            res = safe_get(x) + static_cast<nil::crypto3::word>(y);
                          });
}

int nil_crypto_mp_sub_u32(nil_crypto_mp_t result, const nil_crypto_mp_t x, uint32_t y) {
    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, result, res,
                          {
                              //        if (result == x)
                              //            res -= static_cast<nil::crypto3::word>(y);
                              //        else
                              //            res = safe_get(x) - static_cast<nil::crypto3::word>(y);
                          });
}

int nil_crypto_mp_mul(nil_crypto_mp_t result, const nil_crypto_mp_t x, const nil_crypto_mp_t y) {
    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, result, res,
                          {
                              //        if (result == x)
                              //            res *= safe_get(y);
                              //        else
                              //            res = safe_get(x) * safe_get(y);
                          });
}

int nil_crypto_mp_div(nil_crypto_mp_t quotient, nil_crypto_mp_t remainder, const nil_crypto_mp_t x,
                      const nil_crypto_mp_t y) {
    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, quotient, q,
                          {
                              //        nil::crypto3::multiprecision::cpp_int r;
                              //        nil::crypto3::divide(safe_get(x), safe_get(y), q, r);
                              //        safe_get(remainder) = r;
                          });
}

int nil_crypto_mp_equal(const nil_crypto_mp_t x_w, const nil_crypto_mp_t y_w) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, x_w, x, { return x == safe_get(y_w); });
}

int nil_crypto_mp_is_zero(const nil_crypto_mp_t mp) {
    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, mp, bn, { return bn == 0; });
}

int nil_crypto_mp_is_odd(const nil_crypto_mp_t mp) {
    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, mp, bn, { return bn % 2 == 0; });
}

int nil_crypto_mp_is_even(const nil_crypto_mp_t mp) {
    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, mp, bn, { return !(bn % 2); });
}

int nil_crypto_mp_cmp(int *result, const nil_crypto_mp_t x_w, const nil_crypto_mp_t y_w) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, x_w, x, { *result = x.cmp(safe_get(y_w)); });
}

int nil_crypto_mp_swap(nil_crypto_mp_t x_w, nil_crypto_mp_t y_w) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, x_w, x, { x.swap(safe_get(y_w)); });
}

// Return (base^exponent) % modulus
int nil_crypto_mp_powmod(nil_crypto_mp_t out, const nil_crypto_mp_t base, const nil_crypto_mp_t exponent,
                         const nil_crypto_mp_t modulus) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, out, o,
    //                          { o = nil::crypto3::power_mod(safe_get(base), safe_get(exponent), safe_get(modulus));
    //                          });
}

int nil_crypto_mp_lshift(nil_crypto_mp_t out, const nil_crypto_mp_t in, size_t shift) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, out, o, { o = safe_get(in) << shift; });
}

int nil_crypto_mp_rshift(nil_crypto_mp_t out, const nil_crypto_mp_t in, size_t shift) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, out, o, { o = safe_get(in) >> shift; });
}

int nil_crypto_mp_mod_inverse(nil_crypto_mp_t out, const nil_crypto_mp_t in, const nil_crypto_mp_t modulus) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, out, o,
    //                          { o = ::nil::crypto3::inverse_mod(safe_get(in), safe_get(modulus)); });
}

int nil_crypto_mp_mod_mul(nil_crypto_mp_t out, const nil_crypto_mp_t x, const nil_crypto_mp_t y,
                          const nil_crypto_mp_t modulus) {
    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, out, o,
                          {
                              //        nil::crypto3::modular_reducer reducer(safe_get(modulus));
                              //        o = reducer.multiply(safe_get(x), safe_get(y));
                          });
}

int nil_crypto_mp_rand_bits(nil_crypto_mp_t rand_out, nil_crypto_rng_t rng, size_t bits) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::RandomNumberGenerator, rng, r, { safe_get(rand_out).randomize(r, bits);
    //    });
}

int nil_crypto_mp_rand_range(nil_crypto_mp_t rand_out, nil_crypto_rng_t rng, const nil_crypto_mp_t lower,
                             const nil_crypto_mp_t upper) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::RandomNumberGenerator, rng, r, {
    //        safe_get(rand_out) = nil::crypto3::multiprecision::cpp_int::random_integer(r, safe_get(lower),
    //        safe_get(upper));
    //    });
}

int nil_crypto_mp_gcd(nil_crypto_mp_t out, const nil_crypto_mp_t x, const nil_crypto_mp_t y) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, out, o,
    //                          { o = nil::crypto3::gcd(safe_get(x), safe_get(y)); });
}

int nil_crypto_mp_is_prime(const nil_crypto_mp_t mp, nil_crypto_rng_t rng, size_t test_prob) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, mp, n,
    //                          { return (nil::crypto3::is_prime(n, safe_get(rng), test_prob)) ? 1 : 0; });
}

int nil_crypto_mp_get_bit(const nil_crypto_mp_t mp, size_t bit) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, mp, n, { return (n.get_bit(bit)); });
}

int nil_crypto_mp_set_bit(nil_crypto_mp_t mp, size_t bit) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, mp, n, { n.set_bit(bit); });
}

int nil_crypto_mp_clear_bit(nil_crypto_mp_t mp, size_t bit) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, mp, n, { n.clear_bit(bit); });
}

int nil_crypto_mp_num_bits(const nil_crypto_mp_t mp, size_t *bits) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, mp, n, { *bits = n.bits(); });
}

int nil_crypto_mp_num_bytes(const nil_crypto_mp_t mp, size_t *bytes) {
    //    return CRYPTO3_FFI_DO(nil::crypto3::multiprecision::cpp_int, mp, n, { *bytes = n.bytes(); });
}
}
