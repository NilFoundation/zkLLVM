#include <nil/crypto3/ffi/ffi.h>
#include <nil/crypto3/ffi/ffi_util.h>
#include <nil/crypto3/ffi/ffi_pkey.h>
#include <nil/crypto3/ffi/ffi_rng.h>
#include <nil/crypto3/ffi/ffi_mp.h>

#include  <boost/core/ignore_unused.hpp>

#include <nil/crypto3/pubkey/keys/public_key.hpp>
#include <nil/crypto3/pubkey/keys/private_key.hpp>

#if defined(CRYPTO3_HAS_ECC_PUBLIC_KEY_CRYPTO)
#include <nil/crypto3/pubkey/ecc_key.hpp>
#endif

#if defined(CRYPTO3_HAS_DL_PUBLIC_KEY_FAMILY)
#include <nil/crypto3/pubkey/dl_algorithm.hpp>
#endif

#if defined(CRYPTO3_HAS_RSA)
#include <nil/crypto3/pubkey/rsa/rsa.hpp>
#endif

#if defined(CRYPTO3_HAS_ELGAMAL)
#include <nil/crypto3/pubkey/elgamal/elgamal.hpp>
#endif

#if defined(CRYPTO3_HAS_DSA)
#include <nil/crypto3/pubkey/dsa/dsa.hpp>
#endif

#if defined(CRYPTO3_HAS_ECDSA)
#include <nil/crypto3/pubkey/ecdsa.hpp>
#endif

#if defined(CRYPTO3_HAS_SM2)
#include <nil/crypto3/pubkey/sm2/sm2.hpp>
#include <nil/crypto3/pubkey/sm2/sm2_enc.hpp>
#endif

#if defined(CRYPTO3_HAS_ECDH)
#include <nil/crypto3/pubkey/ecdh/ecdh.hpp>
#endif

#if defined(CRYPTO3_HAS_CURVE_25519)
#include <nil/crypto3/pubkey/curve25519/curve25519.hpp>
#endif

#if defined(CRYPTO3_HAS_ED25519)
#include <nil/crypto3/pubkey/ed25519/ed25519.hpp>
#endif

#if defined(CRYPTO3_HAS_MCELIECE)
#include <nil/crypto3/pubkey/mceliece/mceliece.hpp>
#endif

#if defined(CRYPTO3_HAS_MCEIES)
#include <nil/crypto3/mceies.h>
#endif

#if defined(CRYPTO3_HAS_DIFFIE_HELLMAN)
#include <nil/crypto3/pubkey/dh/dh.hpp>
#endif

namespace {

#if defined(CRYPTO3_HAS_ECC_PUBLIC_KEY_CRYPTO)

    // These are always called within an existing try/catch block

    template<class ECPrivateKey_t>
    int privkey_load_ec(std::unique_ptr<ECPrivateKey_t> &key,
                        const nil::crypto3::multiprecision::cpp_int &scalar,
                        const char *curve_name) {
        if (curve_name == NULL)
            return CRYPTO3_FFI_ERROR_NULL_POINTER;

        nil::crypto3::Null_RNG null_rng;
        nil::crypto3::ec_group grp(curve_name);
        key.reset(new ECPrivateKey_t(null_rng, grp, scalar));
        return CRYPTO3_FFI_SUCCESS;
    }

    template<class ECPublicKey_t>
    int pubkey_load_ec(std::unique_ptr<ECPublicKey_t> &key,
                       const nil::crypto3::multiprecision::cpp_int &public_x,
                       const nil::crypto3::multiprecision::cpp_int &public_y,
                       const char *curve_name) {
        if (curve_name == NULL)
            return CRYPTO3_FFI_ERROR_NULL_POINTER;

        nil::crypto3::ec_group grp(curve_name);
        nil::crypto3::point_gfp uncompressed_point = grp.point(public_x, public_y);
        key.reset(new ECPublicKey_t(grp, uncompressed_point));
        return CRYPTO3_FFI_SUCCESS;
    }

#endif

    template<typename SchemeType>
    nil::crypto3::multiprecision::cpp_int pubkey_get_field(const nil::crypto3::pubkey::public_key<SchemeType> &key,
                                                           const std::string &field) {
        // Maybe this should be `return key.get_integer_field(field_name)`?

#if defined(CRYPTO3_HAS_RSA)
        if (const nil::crypto3::rsa_public_key *rsa = dynamic_cast<const nil::crypto3::rsa_public_key *>(&key)) {
            if (field == "n")
                return rsa->get_n();
            else if (field == "e")
                return rsa->get_e();
            else
                throw nil::crypto3::Exception("Field not supported");
        }
#endif

#if defined(CRYPTO3_HAS_DL_PUBLIC_KEY_FAMILY)
        // Handles DSA, ElGamal, etc
        if (const nil::crypto3::dl_scheme_public_key *dl =
                dynamic_cast<const nil::crypto3::dl_scheme_public_key *>(&key)) {
            if (field == "p")
                return dl->group_p();
            else if (field == "q")
                return dl->group_q();
            else if (field == "g")
                return dl->group_g();
            else if (field == "y")
                return dl->get_y();
            else
                throw nil::crypto3::Exception("Field not supported");
        }
#endif

#if defined(CRYPTO3_HAS_ECC_PUBLIC_KEY_CRYPTO)
        if (const nil::crypto3::ec_public_key *ecc = dynamic_cast<const nil::crypto3::ec_public_key *>(&key)) {
            if (field == "public_x")
                return ecc->public_point().get_affine_x();
            else if (field == "public_y")
                return ecc->public_point().get_affine_y();
            else if (field == "base_x")
                return ecc->domain().get_g_x();
            else if (field == "base_y")
                return ecc->domain().get_g_y();
            else if (field == "p")
                return ecc->domain().get_p();
            else if (field == "a")
                return ecc->domain().get_a();
            else if (field == "b")
                return ecc->domain().get_b();
            else if (field == "cofactor")
                return ecc->domain().get_cofactor();
            else if (field == "order")
                return ecc->domain().get_order();
            else
                throw std::invalid_argument("Field not supported");
        }
#endif

        // Some other algorithm type not supported by this function
        throw std::invalid_argument("Unsupported algorithm type for nil_crypto_pubkey_get_field");
    }

    template<typename SchemeType>
    nil::crypto3::multiprecision::cpp_int privkey_get_field(const nil::crypto3::pubkey::private_key<SchemeType> &key,
                                                            const std::string &field) {
        // return key.get_integer_field(field);

#if defined(CRYPTO3_HAS_RSA)

        if (const nil::crypto3::rsa_private_key *rsa = dynamic_cast<const nil::crypto3::rsa_private_key *>(&key)) {
            if (field == "p")
                return rsa->get_p();
            else if (field == "q")
                return rsa->get_q();
            else if (field == "d")
                return rsa->get_d();
            else if (field == "c")
                return rsa->get_c();
            else if (field == "d1")
                return rsa->get_d1();
            else if (field == "d2")
                return rsa->get_d2();
            else
                return pubkey_get_field(key, field);
        }
#endif

#if defined(CRYPTO3_HAS_DL_PUBLIC_KEY_FAMILY)
        // Handles DSA, ElGamal, etc
        if (const nil::crypto3::dl_scheme_private_key *dl =
                dynamic_cast<const nil::crypto3::dl_scheme_private_key *>(&key)) {
            if (field == "x")
                return dl->get_x();
            else
                return pubkey_get_field(key, field);
        }
#endif

#if defined(CRYPTO3_HAS_ECC_PUBLIC_KEY_CRYPTO)
        if (const nil::crypto3::ec_private_key *ecc = dynamic_cast<const nil::crypto3::ec_private_key *>(&key)) {
            if (field == "x")
                return ecc->private_value();
            else
                return pubkey_get_field(key, field);
        }
#endif

        // Some other algorithm type not supported by this function
        throw std::invalid_argument("Unsupported algorithm type for nil_crypto_privkey_get_field");
    }

}    // namespace

extern "C" {

using namespace nil::crypto3::ffi;

int nil_crypto_pubkey_get_field(nil_crypto_mp_t output, nil_crypto_pubkey_t key, const char *field_name_cstr) {
    if (field_name_cstr == NULL) {
        return CRYPTO3_FFI_ERROR_NULL_POINTER;
    }

    const std::string field_name(field_name_cstr);

//    return CRYPTO3_FFI_DO(nil::crypto3::Public_Key, key, k, { safe_get(output) = pubkey_get_field(k, field_name); });
}

int nil_crypto_privkey_get_field(nil_crypto_mp_t output, nil_crypto_privkey_t key, const char *field_name_cstr) {
    if (field_name_cstr == NULL) {
        return CRYPTO3_FFI_ERROR_NULL_POINTER;
    }

    const std::string field_name(field_name_cstr);

//    return CRYPTO3_FFI_DO(nil::crypto3::private_key, key, k, { safe_get(output) = privkey_get_field(k, field_name); });
}

/* RSA specific operations */

int nil_crypto_privkey_create_rsa(nil_crypto_privkey_t *key_obj, nil_crypto_rng_t rng_obj, size_t n_bits) {
    if (n_bits < 1024 || n_bits > 16 * 1024) {
        return CRYPTO3_FFI_ERROR_BAD_PARAMETER;
    }

    std::string n_str = std::to_string(n_bits);

    return nil_crypto_privkey_create(key_obj, "RSA", n_str.c_str(), rng_obj);
}

int nil_crypto_privkey_load_rsa(nil_crypto_privkey_t *key, nil_crypto_mp_t rsa_p, nil_crypto_mp_t rsa_q,
                                nil_crypto_mp_t rsa_e) {
#if defined(CRYPTO3_HAS_RSA)
    *key = NULL;

    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        *key = new nil_crypto_privkey_struct(
            new nil::crypto3::rsa_private_key(safe_get(rsa_p), safe_get(rsa_q), safe_get(rsa_e)));
        return CRYPTO3_FFI_SUCCESS;
    });
#else
    boost::ignore_unused(key, rsa_p, rsa_q, rsa_e);
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}

int nil_crypto_pubkey_load_rsa(nil_crypto_pubkey_t *key, nil_crypto_mp_t n, nil_crypto_mp_t e) {
#if defined(CRYPTO3_HAS_RSA)
    *key = NULL;
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        *key = new nil_crypto_pubkey_struct(new nil::crypto3::rsa_public_key(safe_get(n), safe_get(e)));
        return CRYPTO3_FFI_SUCCESS;
    });
#else
    boost::ignore_unused(key, n, e);
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}

int nil_crypto_privkey_rsa_get_p(nil_crypto_mp_t p, nil_crypto_privkey_t key) {
    return nil_crypto_privkey_get_field(p, key, "p");
}

int nil_crypto_privkey_rsa_get_q(nil_crypto_mp_t q, nil_crypto_privkey_t key) {
    return nil_crypto_privkey_get_field(q, key, "q");
}

int nil_crypto_privkey_rsa_get_n(nil_crypto_mp_t n, nil_crypto_privkey_t key) {
    return nil_crypto_privkey_get_field(n, key, "n");
}

int nil_crypto_privkey_rsa_get_e(nil_crypto_mp_t e, nil_crypto_privkey_t key) {
    return nil_crypto_privkey_get_field(e, key, "e");
}

int nil_crypto_privkey_rsa_get_d(nil_crypto_mp_t d, nil_crypto_privkey_t key) {
    return nil_crypto_privkey_get_field(d, key, "d");
}

int nil_crypto_pubkey_rsa_get_e(nil_crypto_mp_t e, nil_crypto_pubkey_t key) {
    return nil_crypto_pubkey_get_field(e, key, "e");
}

int nil_crypto_pubkey_rsa_get_n(nil_crypto_mp_t n, nil_crypto_pubkey_t key) {
    return nil_crypto_pubkey_get_field(n, key, "n");
}

/* DSA specific operations */
int nil_crypto_privkey_create_dsa(nil_crypto_privkey_t *key, nil_crypto_rng_t rng_obj, size_t pbits, size_t qbits) {
#if defined(CRYPTO3_HAS_DSA)

    if ((rng_obj == NULL) || (key == NULL))
        return CRYPTO3_FFI_ERROR_NULL_POINTER;

    if ((pbits % 64) || (qbits % 8) || (pbits < 1024) || (pbits > 3072) || (qbits < 160) || (qbits > 256)) {
        return CRYPTO3_FFI_ERROR_BAD_PARAMETER;
    }

    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        nil::crypto3::RandomNumberGenerator &rng = safe_get(rng_obj);
        nil::crypto3::DL_Group group(rng, nil::crypto3::DL_Group::Prime_Subgroup, pbits, qbits);
        *key = new nil_crypto_privkey_struct(new nil::crypto3::dsa_private_key_policy(rng, group));
        return CRYPTO3_FFI_SUCCESS;
    });
#else
    boost::ignore_unused(key, rng_obj, pbits, qbits);
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}

int nil_crypto_privkey_load_dsa(nil_crypto_privkey_t *key, nil_crypto_mp_t p, nil_crypto_mp_t q, nil_crypto_mp_t g,
                                nil_crypto_mp_t x) {
#if defined(CRYPTO3_HAS_DSA)
    *key = NULL;

    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        nil::crypto3::Null_RNG null_rng;
        nil::crypto3::DL_Group group(safe_get(p), safe_get(q), safe_get(g));
        *key = new nil_crypto_privkey_struct(new nil::crypto3::dsa_private_key_policy(null_rng, group, safe_get(x)));
        return CRYPTO3_FFI_SUCCESS;
    });
#else
    boost::ignore_unused(key, p, q, g, x);
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}

int nil_crypto_pubkey_load_dsa(nil_crypto_pubkey_t *key, nil_crypto_mp_t p, nil_crypto_mp_t q, nil_crypto_mp_t g,
                               nil_crypto_mp_t y) {
#if defined(CRYPTO3_HAS_DSA)
    *key = NULL;

    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        nil::crypto3::DL_Group group(safe_get(p), safe_get(q), safe_get(g));
        *key = new nil_crypto_pubkey_struct(new nil::crypto3::dsa_public_key_policy(group, safe_get(y)));
        return CRYPTO3_FFI_SUCCESS;
    });
#else
    boost::ignore_unused(key, p, q, g, y);
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}

int nil_crypto_privkey_dsa_get_x(nil_crypto_mp_t x, nil_crypto_privkey_t key) {
    return nil_crypto_privkey_get_field(x, key, "x");
}

int nil_crypto_pubkey_dsa_get_p(nil_crypto_mp_t p, nil_crypto_pubkey_t key) {
    return nil_crypto_pubkey_get_field(p, key, "p");
}

int nil_crypto_pubkey_dsa_get_q(nil_crypto_mp_t q, nil_crypto_pubkey_t key) {
    return nil_crypto_pubkey_get_field(q, key, "q");
}

int nil_crypto_pubkey_dsa_get_g(nil_crypto_mp_t g, nil_crypto_pubkey_t key) {
    return nil_crypto_pubkey_get_field(g, key, "g");
}

int nil_crypto_pubkey_dsa_get_y(nil_crypto_mp_t y, nil_crypto_pubkey_t key) {
    return nil_crypto_pubkey_get_field(y, key, "y");
}

int nil_crypto_privkey_create_ecdsa(nil_crypto_privkey_t *key_obj, nil_crypto_rng_t rng_obj, const char *param_str) {
    return nil_crypto_privkey_create(key_obj, "ECDSA", param_str, rng_obj);
}

/* ECDSA specific operations */

int nil_crypto_pubkey_load_ecdsa(nil_crypto_pubkey_t *key, const nil_crypto_mp_t public_x,
                                 const nil_crypto_mp_t public_y, const char *curve_name) {
#if defined(CRYPTO3_HAS_ECDSA)
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        std::unique_ptr<nil::crypto3::ecdsa_public_key> p_key;

        int rc = pubkey_load_ec(p_key, safe_get(public_x), safe_get(public_y), curve_name);
        if (rc == CRYPTO3_FFI_SUCCESS)
            *key = new nil_crypto_pubkey_struct(p_key.release());

        return rc;
    });
#else
    boost::ignore_unused(key, public_x, public_y, curve_name);
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}

int nil_crypto_privkey_load_ecdsa(nil_crypto_privkey_t *key, const nil_crypto_mp_t scalar, const char *curve_name) {
#if defined(CRYPTO3_HAS_ECDSA)
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        std::unique_ptr<nil::crypto3::ecdsa_private_key> p_key;
        int rc = privkey_load_ec(p_key, safe_get(scalar), curve_name);
        if (rc == CRYPTO3_FFI_SUCCESS)
            *key = new nil_crypto_privkey_struct(p_key.release());
        return rc;
    });
#else
    boost::ignore_unused(key, scalar, curve_name);
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}

/* ElGamal specific operations */
int nil_crypto_privkey_create_elgamal(nil_crypto_privkey_t *key, nil_crypto_rng_t rng_obj, size_t pbits, size_t qbits) {
#if defined(CRYPTO3_HAS_ELGAMAL)

    if ((rng_obj == NULL) || (key == NULL))
        return CRYPTO3_FFI_ERROR_NULL_POINTER;

    if ((pbits < 1024) || (qbits < 160)) {
        return CRYPTO3_FFI_ERROR_BAD_PARAMETER;
    }

    nil::crypto3::DL_Group::PrimeType prime_type =
        ((pbits - 1) == qbits) ? nil::crypto3::DL_Group::Strong : nil::crypto3::DL_Group::Prime_Subgroup;

    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        nil::crypto3::RandomNumberGenerator &rng = safe_get(rng_obj);
        nil::crypto3::DL_Group group(rng, prime_type, pbits, qbits);
        *key = new nil_crypto_privkey_struct(new nil::crypto3::el_gamal_private_key(rng, group));
        return CRYPTO3_FFI_SUCCESS;
    });
#else
    boost::ignore_unused(key, rng_obj, pbits);
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}

int nil_crypto_pubkey_load_elgamal(nil_crypto_pubkey_t *key, nil_crypto_mp_t p, nil_crypto_mp_t g, nil_crypto_mp_t y) {
#if defined(CRYPTO3_HAS_ELGAMAL)
    *key = NULL;
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        nil::crypto3::DL_Group group(safe_get(p), safe_get(g));
        *key = new nil_crypto_pubkey_struct(new nil::crypto3::el_gamal_public_key(group, safe_get(y)));
        return CRYPTO3_FFI_SUCCESS;
    });
#else
    boost::ignore_unused(key, p, g, y);
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}

int nil_crypto_privkey_load_elgamal(nil_crypto_privkey_t *key, nil_crypto_mp_t p, nil_crypto_mp_t g,
                                    nil_crypto_mp_t x) {
#if defined(CRYPTO3_HAS_ELGAMAL)
    *key = NULL;
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        nil::crypto3::Null_RNG null_rng;
        nil::crypto3::DL_Group group(safe_get(p), safe_get(g));
        *key = new nil_crypto_privkey_struct(new nil::crypto3::el_gamal_private_key(null_rng, group, safe_get(x)));
        return CRYPTO3_FFI_SUCCESS;
    });
#else
    boost::ignore_unused(key, p, g, x);
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}

/* Diffie Hellman specific operations */

int nil_crypto_privkey_create_dh(nil_crypto_privkey_t *key_obj, nil_crypto_rng_t rng_obj, const char *param_str) {
    return nil_crypto_privkey_create(key_obj, "DH", param_str, rng_obj);
}

int nil_crypto_privkey_load_dh(nil_crypto_privkey_t *key, nil_crypto_mp_t p, nil_crypto_mp_t g, nil_crypto_mp_t x) {
#if defined(CRYPTO3_HAS_DIFFIE_HELLMAN)
    *key = NULL;
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        nil::crypto3::Null_RNG null_rng;
        nil::crypto3::DL_Group group(safe_get(p), safe_get(g));
        *key = new nil_crypto_privkey_struct(new nil::crypto3::dh_private_key(null_rng, group, safe_get(x)));
        return CRYPTO3_FFI_SUCCESS;
    });
#else
    boost::ignore_unused(key, p, g, x);
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}

int nil_crypto_pubkey_load_dh(nil_crypto_pubkey_t *key, nil_crypto_mp_t p, nil_crypto_mp_t g, nil_crypto_mp_t y) {
#if defined(CRYPTO3_HAS_DIFFIE_HELLMAN)
    *key = NULL;
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        nil::crypto3::DL_Group group(safe_get(p), safe_get(g));
        *key = new nil_crypto_pubkey_struct(new nil::crypto3::DH_PublicKey(group, safe_get(y)));
        return CRYPTO3_FFI_SUCCESS;
    });
#else
    boost::ignore_unused(key, p, g, y);
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}

/* ECDH + x25519 specific operations */

int nil_crypto_privkey_create_ecdh(nil_crypto_privkey_t *key_obj, nil_crypto_rng_t rng_obj, const char *param_str) {
    if (param_str == NULL) {
        return CRYPTO3_FFI_ERROR_NULL_POINTER;
    }

    const std::string params(param_str);

    if (params == "curve25519") {
        return nil_crypto_privkey_create(key_obj, "Curve25519", "", rng_obj);
    }

    return nil_crypto_privkey_create(key_obj, "ECDH", param_str, rng_obj);
}

int nil_crypto_pubkey_load_ecdh(nil_crypto_pubkey_t *key, const nil_crypto_mp_t public_x,
                                const nil_crypto_mp_t public_y, const char *curve_name) {
#if defined(CRYPTO3_HAS_ECDH)
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        std::unique_ptr<nil::crypto3::ECDH_PublicKey> p_key;
        int rc = pubkey_load_ec(p_key, safe_get(public_x), safe_get(public_y), curve_name);

        if (rc == CRYPTO3_FFI_SUCCESS)
            *key = new nil_crypto_pubkey_struct(p_key.release());
        return rc;
    });
#else
    boost::ignore_unused(key, public_x, public_y, curve_name);
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}

int nil_crypto_privkey_load_ecdh(nil_crypto_privkey_t *key, const nil_crypto_mp_t scalar, const char *curve_name) {
#if defined(CRYPTO3_HAS_ECDH)
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        std::unique_ptr<nil::crypto3::ecdh_private_key> p_key;
        int rc = privkey_load_ec(p_key, safe_get(scalar), curve_name);
        if (rc == CRYPTO3_FFI_SUCCESS)
            *key = new nil_crypto_privkey_struct(p_key.release());
        return rc;
    });
#else
    boost::ignore_unused(key, scalar, curve_name);
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}

/* SM2 specific operations */

int nil_crypto_pubkey_sm2_compute_za(uint8_t out[], size_t *out_len, const char *ident, const char *hash_algo,
                                     const nil_crypto_pubkey_t key) {
    if (out == NULL || out_len == NULL) {
        return CRYPTO3_FFI_ERROR_NULL_POINTER;
    }
    if (ident == NULL || hash_algo == NULL || key == NULL) {
        return CRYPTO3_FFI_ERROR_NULL_POINTER;
    }

#if defined(CRYPTO3_HAS_SM2)
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        const nil::crypto3::Public_Key &pub_key = safe_get(key);
        const nil::crypto3::ec_public_key *ec_key = dynamic_cast<const nil::crypto3::ec_public_key *>(&pub_key);

        if (ec_key == NULL)
            return CRYPTO3_FFI_ERROR_BAD_PARAMETER;

        if (ec_key->algo_name() != "SM2_Sig" && ec_key->algo_name() != "SM2_Enc")
            return CRYPTO3_FFI_ERROR_BAD_PARAMETER;

        const std::string ident_str(ident);
        std::unique_ptr<nil::crypto3::HashFunction> hash = nil::crypto3::HashFunction::create_or_throw(hash_algo);

        const std::vector<uint8_t> za =
            nil::crypto3::sm2_compute_za(*hash, ident_str, ec_key->domain(), ec_key->public_point());

        return write_vec_output(out, out_len, za);
    });
#else
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}

int nil_crypto_pubkey_load_sm2(nil_crypto_pubkey_t *key, const nil_crypto_mp_t public_x, const nil_crypto_mp_t public_y,
                               const char *curve_name) {
#if defined(CRYPTO3_HAS_SM2)
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        std::unique_ptr<nil::crypto3::sm2_signature_public_key> p_key;
        if (!pubkey_load_ec(p_key, safe_get(public_x), safe_get(public_y), curve_name)) {
            *key = new nil_crypto_pubkey_struct(p_key.release());
            return CRYPTO3_FFI_SUCCESS;
        }
        return CRYPTO3_FFI_ERROR_UNKNOWN_ERROR;
    });
#else
    boost::ignore_unused(key, public_x, public_y, curve_name);
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}

int nil_crypto_privkey_load_sm2(nil_crypto_privkey_t *key, const nil_crypto_mp_t scalar, const char *curve_name) {
#if defined(CRYPTO3_HAS_SM2)
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        std::unique_ptr<nil::crypto3::sm2_signature_private_key> p_key;
        int rc = privkey_load_ec(p_key, safe_get(scalar), curve_name);

        if (rc == CRYPTO3_FFI_SUCCESS)
            *key = new nil_crypto_privkey_struct(p_key.release());
        return rc;
    });
#else
    boost::ignore_unused(key, scalar, curve_name);
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}

int nil_crypto_pubkey_load_sm2_enc(nil_crypto_pubkey_t *key, const nil_crypto_mp_t public_x,
                                   const nil_crypto_mp_t public_y, const char *curve_name) {
#if defined(CRYPTO3_HAS_SM2)
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        std::unique_ptr<nil::crypto3::sm2_encryption_public_key> p_key;
        if (!pubkey_load_ec(p_key, safe_get(public_x), safe_get(public_y), curve_name)) {
            *key = new nil_crypto_pubkey_struct(p_key.release());
            return CRYPTO3_FFI_SUCCESS;
        }
        return CRYPTO3_FFI_ERROR_UNKNOWN_ERROR;
    });
#else
    boost::ignore_unused(key, public_x, public_y, curve_name);
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}

int nil_crypto_privkey_load_sm2_enc(nil_crypto_privkey_t *key, const nil_crypto_mp_t scalar, const char *curve_name) {
#if defined(CRYPTO3_HAS_SM2)
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        std::unique_ptr<nil::crypto3::sm2_encryption_private_key> p_key;
        int rc = privkey_load_ec(p_key, safe_get(scalar), curve_name);

        if (rc == CRYPTO3_FFI_SUCCESS)
            *key = new nil_crypto_privkey_struct(p_key.release());
        return rc;
    });
#else
    boost::ignore_unused(key, scalar, curve_name);
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}

/* Ed25519 specific operations */

int nil_crypto_privkey_load_ed25519(nil_crypto_privkey_t *key, const uint8_t privkey[32]) {
#if defined(CRYPTO3_HAS_ED25519)
    *key = NULL;
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        const nil::crypto3::secure_vector<uint8_t> privkey_vec(prkey, prkey + 32);
        *key = new nil_crypto_privkey_struct(new nil::crypto3::Ed25519_PrivateKey(privkey_vec));
        return CRYPTO3_FFI_SUCCESS;
    });
#else
    boost::ignore_unused(key, privkey);
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}

int nil_crypto_pubkey_load_ed25519(nil_crypto_pubkey_t *key, const uint8_t pubkey[32]) {
#if defined(CRYPTO3_HAS_ED25519)
    *key = NULL;
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
        const std::vector<uint8_t> pubkey_vec(pubkey, pubkey + 32);
        *key = new nil_crypto_pubkey_struct(new nil::crypto3::Ed25519_PublicKey(pubkey_vec));
        return CRYPTO3_FFI_SUCCESS;
    });
#else
    boost::ignore_unused(key, pubkey);
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}

int nil_crypto_privkey_ed25519_get_privkey(nil_crypto_privkey_t key, uint8_t output[64]) {
#if defined(CRYPTO3_HAS_ED25519)
    return CRYPTO3_FFI_DO(nil::crypto3::private_key, key, k, {
        if (nil::crypto3::Ed25519_PrivateKey *ed = dynamic_cast<nil::crypto3::Ed25519_PrivateKey *>(&k)) {
            const nil::crypto3::secure_vector<uint8_t> &ed_key = ed->get_private_key();
            if (ed_key.size() != 64)
                return CRYPTO3_FFI_ERROR_INSUFFICIENT_BUFFER_SPACE;
            nil::crypto3::copy_mem(output, ed_key.data(), ed_key.size());
            return CRYPTO3_FFI_SUCCESS;
        } else {
            return CRYPTO3_FFI_ERROR_BAD_PARAMETER;
        }
    });
#else
    boost::ignore_unused(key, output);
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}

int nil_crypto_pubkey_ed25519_get_pubkey(nil_crypto_pubkey_t key, uint8_t output[32]) {
#if defined(CRYPTO3_HAS_ED25519)
    return CRYPTO3_FFI_DO(nil::crypto3::Public_Key, key, k, {
        if (nil::crypto3::Ed25519_PublicKey *ed = dynamic_cast<nil::crypto3::Ed25519_PublicKey *>(&k)) {
            const std::vector<uint8_t> &ed_key = ed->get_public_key();
            if (ed_key.size() != 32)
                return CRYPTO3_FFI_ERROR_INSUFFICIENT_BUFFER_SPACE;
            nil::crypto3::copy_mem(output, ed_key.data(), ed_key.size());
            return CRYPTO3_FFI_SUCCESS;
        } else {
            return CRYPTO3_FFI_ERROR_BAD_PARAMETER;
        }
    });
#else
    boost::ignore_unused(key, output);
    return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
}

int nil_crypto_privkey_create_mceliece(nil_crypto_privkey_t *key_obj, nil_crypto_rng_t rng_obj, size_t n, size_t t) {
    const std::string mce_params = std::to_string(n) + "," + std::to_string(t);
    return nil_crypto_privkey_create(key_obj, "McEliece", mce_params.c_str(), rng_obj);
}

int nil_crypto_mceies_decrypt(nil_crypto_privkey_t mce_key_obj, const char *aead, const uint8_t ct[], size_t ct_len,
                              const uint8_t ad[], size_t ad_len, uint8_t out[], size_t *out_len) {
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
//        nil::crypto3::private_key &key = safe_get(mce_key_obj);

#if defined(CRYPTO3_HAS_MCELIECE) && defined(CRYPTO3_HAS_MCEIES)
        nil::crypto3::mceliece_private_key *mce = dynamic_cast<nil::crypto3::mceliece_private_key *>(&key);
        if (!mce)
            return CRYPTO3_FFI_ERROR_BAD_PARAMETER;

        const nil::crypto3::secure_vector<uint8_t> pt = mceies_decrypt(*mce, ct, ct_len, ad, ad_len, aead);
        return write_vec_output(out, out_len, pt);
#else
        return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
    });
}

int nil_crypto_mceies_encrypt(nil_crypto_pubkey_t mce_key_obj, nil_crypto_rng_t rng_obj, const char *aead,
                              const uint8_t pt[], size_t pt_len, const uint8_t ad[], size_t ad_len, uint8_t out[],
                              size_t *out_len) {
    return ffi_guard_thunk(CRYPTO3_CURRENT_FUNCTION, [=]() -> int {
//        nil::crypto3::Public_Key &key = safe_get(mce_key_obj);
//        nil::crypto3::RandomNumberGenerator &rng = safe_get(rng_obj);

#if defined(CRYPTO3_HAS_MCELIECE) && defined(CRYPTO3_HAS_MCEIES)
        nil::crypto3::McEliece_PublicKey *mce = dynamic_cast<nil::crypto3::McEliece_PublicKey *>(&key);
        if (!mce)
            return CRYPTO3_FFI_ERROR_BAD_PARAMETER;

        nil::crypto3::secure_vector<uint8_t> ct = mceies_encrypt(*mce, pt, pt_len, ad, ad_len, rng, aead);
        return write_vec_output(out, out_len, ct);
#else
        return CRYPTO3_FFI_ERROR_NOT_IMPLEMENTED;
#endif
    });
}
}
