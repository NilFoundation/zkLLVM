#include <nil/crypto3/ffi/ffi.h>
#include <nil/crypto3/ffi/ffi_util.h>

#include <iostream>

#include <nil/crypto3/block/detail/utilities/memory_operations.hpp>

#include <nil/crypto3/codec/hex.hpp>
#include <nil/crypto3/codec/base.hpp>

#include <nil/crypto3/codec/algorithm/encode.hpp>
#include <nil/crypto3/codec/algorithm/decode.hpp>

namespace nil {
    namespace crypto3 {
        namespace ffi {

            int ffi_error_exception_thrown(const char *func_name, const char *exn) {
                fprintf(stderr, "in %s exception %s\n", func_name, exn);
                return CRYPTO3_FFI_ERROR_EXCEPTION_THROWN;
            }

            int write_output(uint8_t *out, size_t *out_len, const uint8_t *buf, size_t buf_len) {
                const size_t avail = *out_len;
                *out_len = buf_len;

                if (avail >= buf_len) {
                    nil::crypto3::copy_mem(out, buf, buf_len);
                    return CRYPTO3_FFI_SUCCESS;
                } else {
                    nil::crypto3::clear_mem(out, avail);
                    return CRYPTO3_FFI_ERROR_INSUFFICIENT_BUFFER_SPACE;
                }
            }
            int write_str_output(uint8_t *out, size_t *out_len, const std::string &str) {
                return write_output(out, out_len, nil::crypto3::cast_char_ptr_to_uint8(str.data()), str.size() + 1);
            }
            int write_str_output(char *out, size_t *out_len, const std::string &str) {
                return write_str_output(nil::crypto3::cast_char_ptr_to_uint8(out), out_len, str);
            }
            int write_str_output(char *out, size_t *out_len, const std::vector<uint8_t> &str_vec) {
                return write_output(nil::crypto3::cast_char_ptr_to_uint8(out), out_len, str_vec.data(), str_vec.size());
            }
        }    // namespace ffi
    }        // namespace crypto3
}    // namespace nil

extern "C" {

using namespace nil::crypto3::ffi;
using namespace nil::crypto3;

const char *nil_crypto_error_description(int err) {
    switch (err) {
        case CRYPTO3_FFI_SUCCESS:
            return "OK";

        case CRYPTO3_FFI_INVALID_VERIFIER:
            return "Invalid verifier";

        case CRYPTO3_FFI_ERROR_INVALID_INPUT:
            return "Invalid input";

        case CRYPTO3_FFI_ERROR_BAD_MAC:
            return "Invalid authentication code";

        case CRYPTO3_FFI_ERROR_INSUFFICIENT_BUFFER_SPACE:
            return "Insufficient buffer space";

        case CRYPTO3_FFI_ERROR_EXCEPTION_THROWN:
            return "Exception thrown";

        case CRYPTO3_FFI_ERROR_BAD_FLAG:
            return "Bad flag";

        case CRYPTO3_FFI_ERROR_NULL_POINTER:
            return "Null pointer argument";

        case CRYPTO3_FFI_ERROR_BAD_PARAMETER:
            return "Bad parameter";

        case CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED:
            return "Not implemented";

        case CRYPTO3_FFI_ERROR_INVALID_OBJECT:
            return "Invalid object handle";

        case CRYPTO3_FFI_ERROR_UNKNOWN_ERROR:
            return "Unknown error";
        default:
            break;
    }

    return "Unknown error";
}

int nil_crypto_hex_encode(const uint8_t *in, size_t len, char *out, uint32_t flags) {
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        const bool uppercase = (flags & CRYPTO3_FFI_HEX_LOWER_CASE) == 0;
        if (uppercase) {
            encode<codec::hex<codec::mode::upper>>(in, in + len, out);
        } else {
            encode<codec::hex<codec::mode::lower>>(in, in + len, out);
        }
        return CRYPTO3_FFI_SUCCESS;
    });
}

int nil_crypto_hex_decode(const char *hex_str, size_t in_len, uint8_t *out, size_t *out_len) {
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        const std::vector<uint8_t> bin = decode<codec::hex<>>(hex_str, hex_str + in_len);
        return ffi::write_vec_output(out, out_len, bin);
    });
}

int nil_crypto_base64_encode(const uint8_t *in, size_t len, char *out, size_t *out_len) {
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        const std::string base64 = encode<codec::base<64>>(in, in + len);
        return ffi::write_str_output(out, out_len, base64);
    });
}

int nil_crypto_base64_decode(const char *base64_str, size_t in_len, uint8_t *out, size_t *out_len) {
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
//        if (*out_len < nil::crypto3::base64_decode_max_output(in_len)) {
//            *out_len = nil::crypto3::base64_decode_max_output(in_len);
//            return CRYPTO3_FFI_ERROR_INSUFFICIENT_BUFFER_SPACE;
//        }

        decode<codec::base<64>>(base64_str, base64_str + in_len, out);
        return CRYPTO3_FFI_SUCCESS;
    });
}
}