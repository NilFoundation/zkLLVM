// implementation of class polynomial: https://github.com/nilfoundation/crypto3-math/blob/5f92125/include/nil/crypto3/math/polynomial/polynomial.hpp

#include <nil/crypto3/algebra/curves/pallas.hpp>
#include <array>

using namespace nil::crypto3::algebra::curves;

template<std::size_t length>
class polynomial {

public:
    typedef typename pallas::base_field_type FieldType;

    typedef std::array<FieldType::value_type, length> container_type;

    std::array<typename FieldType::value_type, length> val;

    typedef typename container_type::value_type value_type;
    typedef typename container_type::reference reference;
    typedef typename container_type::const_reference const_reference;
    typedef typename container_type::size_type size_type;
    typedef typename container_type::iterator iterator;
    typedef typename container_type::const_iterator const_iterator;


    typename FieldType::value_type evaluate(typename FieldType::value_type value) {
        typename FieldType::value_type result = 0;
        for (std::size_t i = 0; i < length; i++) {
            result = result * value + val[length - 1 - i];
        }
        // auto end = val.end();
        // while (end != val.begin()) {
        //     result = result * value + *--end;
        // }
        return result;
    }

    polynomial& operator=(const polynomial& x) {
        val = x.val;
        return *this;
    }

    polynomial operator+(const polynomial other) {
        polynomial<length> result;
        for (std::size_t i = 0; i < length; i++) {
            result.val[i] = this->val[i] + other.val[i];
        }
        return result;
    }

    polynomial operator-(const polynomial other) {
        polynomial<length> result;
        for (std::size_t i = 0; i < length; i++) {
            result.val[i] = this->val[i] - other.val[i];
        }
        return result;
    }

    polynomial operator*(const polynomial& other) const {
        polynomial result;
        multiplication<FieldType, length>(result, *this, other);
        return result;
    }

    polynomial(typename FieldType::value_type x) {
        for (std::size_t i = 0; i < length; i++) {
            val[i] = x;
        }
    }

    polynomial() {
        typename FieldType::value_type zero = typename FieldType::value_type(0); 
        for (std::size_t i = 0; i < length; i++) {
            val[i] = zero;
        }
    }

    reference operator[](size_type _n) BOOST_NOEXCEPT {
        return val[_n];
    }
    const_reference operator[](size_type _n) const BOOST_NOEXCEPT {
        return val[_n];
    }


    iterator begin() BOOST_NOEXCEPT {
        return val.begin();
    }
    const_iterator begin() const BOOST_NOEXCEPT {
        return val.begin();
    }
    iterator end() BOOST_NOEXCEPT {
        return val.end();
    }
    const_iterator end() const BOOST_NOEXCEPT {
        return val.end();
    }

    size_type size() const BOOST_NOEXCEPT {
        return val.size();
    }

    reference back() BOOST_NOEXCEPT {
        return val.back();
    }
    const_reference back() const BOOST_NOEXCEPT {
        return val.back();
    }

    bool is_zero() const {
        return std::all_of(this->begin(), this->end(),
                           [](typename FieldType::value_type i) { return i == typename FieldType::value_type(0); });
    }

};

std::size_t bitreverse(std::size_t n, const std::size_t l) {
    std::size_t r = 0;
    for (std::size_t k = 0; k < l; ++k) {
        r = (r << 1) | (n & 1);
        n >>= 1;
    }
    return r;
}

template <typename FieldType, std::size_t length>
void basic_radix2_fft(
    polynomial<length> &a,
    const typename FieldType::value_type &omega
) {
    typedef typename FieldType::value_type field_value_type;
    typedef typename std::iterator_traits<decltype(std::begin(std::declval<polynomial<length>>()))>::value_type value_type;

    const std::size_t n = length, logn = log2(n);
    if (n != (1u << logn))
        throw std::invalid_argument("expected n == (1u << logn)");

    for (std::size_t k = 0; k < n; ++k) {
        const std::size_t rk = bitreverse(k, logn);
        if (k < rk)
            std::swap(a[k], a[rk]);
    }

    std::size_t m = 1;    // invariant: m = 2^{s-1}
    for (std::size_t s = 1; s <= logn; ++s) {
        const field_value_type w_m = omega; // TODO: must be = omega.pow(n / (2 * m)); , .pow is not implemented yet 

        asm volatile("/* pre-inner */");
        for (std::size_t k = 0; k < n; k += 2 * m) {
            field_value_type w = 1;
            for (std::size_t j = 0; j < m; ++j) {
                const value_type t = a[k + j + m] * w;
                a[k + j + m] = a[k + j] - t;
                a[k + j] = a[k + j] + t;
                w *= w_m;
            }
        }
        asm volatile("/* post-inner */");
        m *= 2;
    }
}

constexpr std::size_t power_of_two(std::size_t n) {
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    return n;
}

template<typename FieldType>
constexpr typename FieldType::value_type unity_root(const std::size_t n) {
    
    typedef typename FieldType::value_type value_type;

    const std::size_t logn = std::ceil(std::log2(n));

    if (n != (1u << logn)) {
        throw std::invalid_argument("expected n == (1u << logn)");
    }
    if (logn > 0x20 /*pallas s*/) {
        throw std::invalid_argument("expected logn <= arithmetic_params<FieldType>::s");
    }

    value_type omega = value_type(0x1ea14637cbe1870c65d520c6cd47d259883000713dc3c2a1adf8b071592f247a_cppui255); // pallas root of unity
    for (std::size_t i = 0x20/*pallas s*/; i > logn; --i) {
        omega *= omega;
    }
    return omega;
}

template<typename FieldType, std::size_t length>
void multiplication(polynomial<length> &c, const polynomial<length> &a, const polynomial<length> &b) {

    typedef typename std::iterator_traits<decltype(std::begin(std::declval<polynomial<length>>()))>::value_type field_value_type;

    const std::size_t n = power_of_two(a.size() + b.size() - 1);
    field_value_type omega = unity_root<FieldType>(n);

    polynomial<length> u(a);
    polynomial<length> v(b);

    basic_radix2_fft<FieldType, length>(u, omega);
    basic_radix2_fft<FieldType, length>(v, omega);

    for(std::size_t i = 0; i < n; ++i) {
        c[i] = u[i] * v[i];
    }

    basic_radix2_fft<FieldType, length>(c, 1 / omega);

    const field_value_type sconst = field_value_type(1) / field_value_type(n);
                
    for(std::size_t i = 0; i < n; ++i) {
        c[i] = c[i] * sconst;
    }
                
    // condense(c);
}

template<std::size_t length>
bool is_zero(const polynomial<length> &a) {
    typename polynomial<length>::value_type zero =
        typename std::iterator_traits<decltype(std::begin(std::declval<polynomial<length>>()))>::value_type();
    return !std::any_of(
        std::begin(a),
        std::end(a),
        [zero](typename std::iterator_traits<decltype(std::begin(std::declval<polynomial<length>>()))>::value_type i) {
            return i != zero;
        });
}

template<typename FieldType, std::size_t length>
void division(polynomial<length> &q, polynomial<length> &r, const polynomial<length> &a, const polynomial<length> &b) {

    typedef typename std::iterator_traits<decltype(std::begin(std::declval<polynomial<length>>()))>::value_type value_type;

    std::size_t d = b.size() - 1;

    // if (b.back() == 1 && is_zero<length>(b.begin() + 1, b.end() - 1) && a.size() >= b.size()) { // TODO not implemented yet
    //     q = polynomial<length>(a.size() - b.size() + 1, 0);
    //     r = polynomial<length>(a.begin(), a.end() - (a.size() - b.size() + 1));
    if (false) { 
        r = polynomial<length>(a);
        q = polynomial<length>();

        value_type x = -b[0];
        auto end = a.end() - 1;
        for (std::size_t t = q.size(); t != 0; --t, --end) {
            q[t - 1] += *end;
            if (t - 1 >= d) {
                q[t - 1 - d] = q[t - 1] * x;
            } else {
                r[t - 1] += q[t - 1] * x;
            }
        }
    } else {
        value_type c = 1 / b.back();
        r = polynomial<length>(a);
        q = polynomial<length>();

        std::size_t r_deg = r.size() - 1;
        std::size_t shift;

        while (r_deg >= d && !is_zero(r)) {
            shift = r_deg - d;

            value_type lead_coeff = r.back() * c;
            q[shift] = lead_coeff;

            // if (b.size() + shift + 1 > r.size()) 
            //     r.resize(b.size() + shift + 1); // TODO not implemented yet
            auto glambda = [=](value_type x, value_type y) { return y - (x * lead_coeff); };
            std::transform(b.begin(), b.end(), r.begin() + shift, r.begin() + shift, glambda);

            r_deg = r.size() - 1;
        }
    }
}

[[circuit]] typename pallas::base_field_type::value_type
    poly_calc(typename pallas::base_field_type::value_type a,
              typename pallas::base_field_type::value_type b) {
    const std::size_t len = 5;

    polynomial<len> P(a);
    P.val[0] = a;
    P.val[1] = b;
    a = P.evaluate(b);
    polynomial<len> Q(a);
    polynomial<len> Z(b);
    polynomial<len> R(b);

    Z = Q;

    Z = Q + P;

    a = Z.evaluate(b);

    basic_radix2_fft<typename pallas::base_field_type, len> (P, a);

    multiplication<typename pallas::base_field_type, len> (P, Q, Z);
    division<typename pallas::base_field_type, len> (P, Q, Z, R);

    Z = P * Q;

    a = Z.evaluate(b);

    return a + b;
}