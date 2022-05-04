#ifndef CRYPTO3_FFI_MP_HPP
#define CRYPTO3_FFI_MP_HPP

#include <nil/crypto3/ffi/ffi_util.h>

#include <nil/crypto3/multiprecision/cpp_int.hpp>

extern "C" {

CRYPTO3_FFI_DECLARE_STRUCT(nil_crypto_mp_struct, nil::crypto3::multiprecision::cpp_int, 0xC828B9D2);
}

#endif
