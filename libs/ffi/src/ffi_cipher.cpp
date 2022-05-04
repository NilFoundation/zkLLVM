#include <nil/crypto3/ffi/ffi.h>
#include <nil/crypto3/ffi/ffi_util.h>

#include <boost/current_function.hpp>

extern "C" {

using namespace nil::crypto3::ffi;

//struct nil_crypto_cipher_struct final : public nil_crypto_struct<nil::crypto3::cipher_mode, 0xB4A2BF9C> {
//    explicit nil_crypto_cipher_struct(nil::crypto3::cipher_mode *x) : nil_crypto_struct(x) {
//    }
//
//    nil::crypto3::secure_vector<uint8_t> m_buf;
//};

int nil_crypto_cipher_init(nil_crypto_cipher_t *cipher, const char *cipher_name, uint32_t flags) {
    return ffi_guard_thunk(BOOST_CURRENT_FUNCTION, [=]() -> int {
        const bool encrypt_p = ((flags & CRYPTO3_CIPHER_INIT_FLAG_MASK_DIRECTION) == CRYPTO3_CIPHER_INIT_FLAG_ENCRYPT);
//        const nil::crypto3::Cipher_Dir dir = encrypt_p ? nil::crypto3::ENCRYPTION : nil::crypto3::DECRYPTION;
//        std::unique_ptr<nil::crypto3::cipher_mode> mode(nil::crypto3::cipher_mode::create(cipher_name, dir));
//        if (!mode) {
//            return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
//        }
//        *cipher = new nil_crypto_cipher_struct(mode.release());
        return CRYPTO3_FFI_SUCCESS;
    });
}

int nil_crypto_cipher_destroy(nil_crypto_cipher_t cipher) {
//    return CRYPTO3_FFI_CHECKED_DELETE(cipher);
}

int nil_crypto_cipher_clear(nil_crypto_cipher_t cipher) {
//    return CRYPTO3_FFI_DO(nil::crypto3::cipher_mode, cipher, c, { c.clear(); });
}

int nil_crypto_cipher_query_keylen(nil_crypto_cipher_t cipher, size_t *out_minimum_keylength,
                                   size_t *out_maximum_keylength) {
//    return CRYPTO3_FFI_DO(nil::crypto3::cipher_mode, cipher, c, {
//        *out_minimum_keylength = c.key_spec().minimum_keylength();
//        *out_maximum_keylength = c.key_spec().maximum_keylength();
//    });
}

int nil_crypto_cipher_set_key(nil_crypto_cipher_t cipher, const uint8_t *key, size_t key_len) {
//    return CRYPTO3_FFI_DO(nil::crypto3::cipher_mode, cipher, c, { c.set_key(key, key_len); });
}

int nil_crypto_cipher_start(nil_crypto_cipher_t cipher_obj, const uint8_t *nonce, size_t nonce_len) {
    return ffi_guard_thunk(BOOST_CURRENT_FUNCTION, [=]() -> int {
//        nil::crypto3::cipher_mode &cipher = safe_get(cipher_obj);
//        cipher.start(nonce, nonce_len);
//        cipher_obj->m_buf.reserve(cipher.update_granularity());
        return CRYPTO3_FFI_SUCCESS;
    });
}

int nil_crypto_cipher_update(nil_crypto_cipher_t cipher_obj, uint32_t flags, uint8_t output_ptr[],
                             size_t orig_output_size, size_t *output_written, const uint8_t input_ptr[],
                             size_t orig_input_size, size_t *input_consumed) {
    return ffi_guard_thunk(BOOST_CURRENT_FUNCTION, [=]() -> int {
        size_t input_size = orig_input_size;
        size_t output_size = orig_output_size;
        const uint8_t *input = input_ptr;
        uint8_t *output = output_ptr;

//        using namespace nil::crypto3;
//        cipher_mode &cipher = safe_get(cipher_obj);
//        secure_vector<uint8_t> &mbuf = cipher_obj->m_buf;

        const bool final_input = (flags & CRYPTO3_CIPHER_UPDATE_FLAG_FINAL);

        if (final_input) {
//            mbuf.assign(input, input + input_size);
            *input_consumed = input_size;
            *output_written = 0;

//            try {
//                cipher.finish(mbuf);
//            } catch (integrity_failure &) {
//                return CRYPTO3_FFI_ERROR_BAD_MAC;
//            }

//            *output_written = mbuf.size();

//            if (mbuf.size() <= output_size) {
//                copy_mem(output, mbuf.data(), mbuf.size());
//                mbuf.clear();
//                return CRYPTO3_FFI_SUCCESS;
//            }

            return -1;
        }

        if (input_size == 0) {
            // Currently must take entire buffer in this case
//            *output_written = mbuf.size();
//            if (output_size >= mbuf.size()) {
//                copy_mem(output, mbuf.data(), mbuf.size());
//                mbuf.clear();
//                return CRYPTO3_FFI_SUCCESS;
//            }

            return -1;
        }

//        const size_t ud = cipher.update_granularity();
//        BOOST_ASSERT_MSG(cipher.update_granularity() > cipher.minimum_final_size(), "logic error");

//        mbuf.resize(ud);
        size_t taken = 0, written = 0;

//        while (input_size >= ud && output_size >= ud) {
//            copy_mem(mbuf.data(), input, ud);
//            cipher.update(mbuf);
//
//            input_size -= ud;
//            copy_mem(output, mbuf.data(), ud);
//            input += ud;
//            taken += ud;
//
//            output_size -= ud;
//            output += ud;
//            written += ud;
//        }

        *output_written = written;
        *input_consumed = taken;

        return CRYPTO3_FFI_SUCCESS;
    });
}

int nil_crypto_cipher_set_associated_data(nil_crypto_cipher_t cipher, const uint8_t *ad, size_t ad_len) {
//    return CRYPTO3_FFI_DO(nil::crypto3::cipher_mode, cipher, c, {
//        if (nil::crypto3::AEAD_Mode *aead = dynamic_cast<nil::crypto3::AEAD_Mode *>(&c)) {
//            aead->set_associated_data(ad, ad_len);
//            return CRYPTO3_FFI_SUCCESS;
//        }
//        return CRYPTO3_FFI_ERROR_BAD_PARAMETER;
//    });
}

int nil_crypto_cipher_valid_nonce_length(nil_crypto_cipher_t cipher, size_t nl) {
//    return CRYPTO3_FFI_DO(nil::crypto3::cipher_mode, cipher, c, { return c.valid_nonce_length(nl) ? 1 : 0; });
}

int nil_crypto_cipher_get_default_nonce_length(nil_crypto_cipher_t cipher, size_t *nl) {
//    return CRYPTO3_FFI_DO(nil::crypto3::cipher_mode, cipher, c, { *nl = c.default_nonce_length(); });
}

int nil_crypto_cipher_get_update_granularity(nil_crypto_cipher_t cipher, size_t *ug) {
//    return CRYPTO3_FFI_DO(nil::crypto3::cipher_mode, cipher, c, { *ug = c.update_granularity(); });
}

int nil_crypto_cipher_get_tag_length(nil_crypto_cipher_t cipher, size_t *tl) {
//    return CRYPTO3_FFI_DO(nil::crypto3::cipher_mode, cipher, c, { *tl = c.tag_size(); });
}
}
