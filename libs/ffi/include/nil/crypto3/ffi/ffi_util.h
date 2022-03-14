#ifndef CRYPTO3_FFI_UTILS_HPP
#define CRYPTO3_FFI_UTILS_HPP

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>

namespace nil {
    namespace crypto3 {
        namespace ffi {
            template<typename T, uint32_t MAGIC>
            struct nil_crypto_struct {
            public:
                nil_crypto_struct(T *obj) : m_magic(MAGIC), m_obj(obj) {
                }

                virtual ~nil_crypto_struct() {
                    m_magic = 0;
                    m_obj.reset();
                }

                bool magic_ok() const {
                    return (m_magic == MAGIC);
                }

                T *unsafe_get() const {
                    return m_obj.get();
                }

            private:
                uint32_t m_magic;
                std::unique_ptr<T> m_obj;
            };

#define CRYPTO3_FFI_DECLARE_STRUCT(NAME, TYPE, MAGIC)                              \
    struct NAME final : public nil::crypto3::ffi::nil_crypto_struct<TYPE, MAGIC> { \
        explicit NAME(TYPE *x) : nil_crypto_struct(x) {                            \
        }                                                                          \
    }

            // Declared in ffi.cpp
            int ffi_error_exception_thrown(const char *func_name, const char *exn);

            template<typename T, uint32_t M>
            T &safe_get(nil_crypto_struct<T, M> *p) {
                if (!p) {
                    throw std::invalid_argument("Null pointer argument");
                }
                if (!p->magic_ok()) {
                    throw std::invalid_argument("Bad magic in ffi object");
                }

                T *t = p->unsafe_get();
                if (t) {
                    return *t;
                } else {
                    throw std::invalid_argument("Invalid object pointer");
                }
            }

            template<typename Thunk>
            int ffi_guard_thunk(const char *func_name, Thunk thunk) {
                try {
                    return thunk();
                } catch (std::bad_alloc &) {
                    return ffi_error_exception_thrown(func_name, "bad_alloc");
                } catch (std::exception &e) {
                    return ffi_error_exception_thrown(func_name, e.what());
                } catch (...) {
                    return ffi_error_exception_thrown(func_name, "unknown exception");
                }

                return CRYPTO3_FFI_ERROR_UNKNOWN_ERROR;
            }

            template<typename T, uint32_t M, typename F>
            int apply_fn(nil_crypto_struct<T, M> *o, const char *func_name, F func) {
                if (!o) {
                    return CRYPTO3_FFI_ERROR_NULL_POINTER;
                }

                if (!o->magic_ok()) {
                    return CRYPTO3_FFI_ERROR_INVALID_OBJECT;
                }

                return ffi_guard_thunk(func_name, [&]() { return func(*o->unsafe_get()); });
            }

#define CRYPTO3_FFI_DO(T, obj, param, block)                       \
    apply_fn(obj, CRYPTO3_CURRENT_FUNCTION, [=](T &param) -> int { \
        do {                                                       \
            block                                                  \
        } while (0);                                               \
        return CRYPTO3_FFI_SUCCESS;                                \
    })

            template<typename T, uint32_t M>
            int ffi_delete_object(nil_crypto_struct<T, M> *obj, const char *func_name) {
                try {
                    if (obj == nullptr) {
                        return CRYPTO3_FFI_SUCCESS;
                    }    // ignore delete of null objects

                    if (!obj->magic_ok()) {
                        return CRYPTO3_FFI_ERROR_INVALID_OBJECT;
                    }

                    delete obj;
                    return CRYPTO3_FFI_SUCCESS;
                } catch (std::exception &e) {
                    return ffi_error_exception_thrown(func_name, e.what());
                } catch (...) {
                    return ffi_error_exception_thrown(func_name, "unknown exception");
                }
            }

#define CRYPTO3_FFI_CHECKED_DELETE(o) ffi_delete_object(o, CRYPTO3_CURRENT_FUNCTION)

            inline int write_output(uint8_t out[], size_t *out_len, const uint8_t buf[], size_t buf_len);

            template<typename Alloc>
            int write_vec_output(uint8_t out[], size_t *out_len, const std::vector<uint8_t, Alloc> &buf) {
                return write_output(out, out_len, buf.data(), buf.size());
            }

            inline int write_str_output(uint8_t out[], size_t *out_len, const std::string &str);

            inline int write_str_output(char out[], size_t *out_len, const std::string &str);

            inline int write_str_output(char out[], size_t *out_len, const std::vector<uint8_t> &str_vec);

        }    // namespace ffi
    }        // namespace crypto3
}    // namespace nil

#endif
