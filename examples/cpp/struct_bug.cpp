#pragma once

#include <array>
#include <cstdint>
#include <initializer_list>

using Byte = unsigned char;

template<typename T, std::size_t CAPACITY = 128, bool INITIALLY_FULL = false>
struct static_vector {

    T content_[CAPACITY];
    size_t size_;

    constexpr static_vector(std::initializer_list<T> init) {
        size_ = 0;
        for(const auto& v : init) {
            content_[size_++] = v;
        }
    }

    template<std::size_t SIZE>
    constexpr explicit static_vector(const std::array<T, SIZE>& rhs) {
        static_assert(SIZE <= CAPACITY);
        for(size_t i = 0; i < SIZE; i++) {
            content_[i] = rhs[i];
        }
        size_ = SIZE;
    }
    template<std::size_t SIZE, bool FULL>
    constexpr explicit static_vector(const static_vector<T, SIZE, FULL>& rhs) {
        static_assert(SIZE <= CAPACITY);
        for(size_t i = 0; i < SIZE; i++) {
            content_[i] = rhs[i];
        }
        size_ = SIZE;
    }
    constexpr static_vector() {
        if constexpr (INITIALLY_FULL) {
            size_ = CAPACITY;
        } else {
            size_ = 0;
        }
    }
    constexpr static_vector(const static_vector& rhs) {
        for(size_t i = 0; i < rhs.size_; i++) {
            content_[i] = rhs[i];
        }
        size_ = rhs.size_;
    }
    // For some reason, this triggers a circuit compilation error
    // ~static_vector() {
    //     size_ = 0;
    // }
    constexpr auto operator=(const static_vector& rhs) -> static_vector& {
        for(size_t i = 0; i < rhs.size_; i++) {
            content_[i] = rhs[i];
        }
        size_ = rhs.size_;
        return *this;
    }
    constexpr auto data() -> T* {
        return &content_;
    }
    constexpr auto content() -> std::array<T, CAPACITY>& {
        return reinterpret_cast<std::array<T, CAPACITY>&>(content_);
    }
    constexpr auto operator[](std::size_t index) -> T& {
        return content_[index];
    }
    constexpr auto operator[](std::size_t index) const -> const T& {
        const T& retval = content_[index];
        return retval;
    }
    constexpr auto size() const {
        return size_;
    }
    constexpr auto capacity() -> std::size_t {
        return CAPACITY;
    }
    constexpr auto push_back(T const& val) -> void {
        content_[size_++] = val;
    }
    constexpr void clear() {
        size_ = 0;
    }
    constexpr bool full() const {
        return CAPACITY == size_;
    }
    constexpr auto pop_back(T const& val) -> void {
        --size_;
    }
    bool operator==(const static_vector& rhs) const {
        if(size_ != rhs.size_) {
            return false;
        }
        for(size_t i = 0; i < size_; i++) {
            if(content_[i] != rhs[i]) {
                return false;
            }
        }
        return true;
    }
    bool operator!=(const static_vector& rhs) const {
        return !(*this == rhs);
    }
} __attribute__((packed));


template<std::size_t CAPACITY>
struct static_vector<Byte, CAPACITY> {
    Byte content_[CAPACITY];

    constexpr static_vector(std::initializer_list<Byte> init) {
        size_t i = 0;
        for(const auto& v : init) {
            content_[i++] = v;
        }
    }

    template<std::size_t SIZE>
    constexpr explicit static_vector(std::array<Byte, SIZE> const& rhs) {
        static_assert(SIZE <= CAPACITY);
        for(size_t i = 0; i < SIZE; i++) {
            content_[i] = rhs[i];
        }
    }
    template<std::size_t SIZE>
    constexpr explicit static_vector(static_vector<Byte, SIZE> const& rhs) {
        static_assert(SIZE <= CAPACITY);
        for(size_t i = 0; i < SIZE; i++) {
            content_[i] = rhs[i];
        }
    }
    constexpr static_vector() {
        for(size_t i = 0; i < CAPACITY; i++) {
            content_[i] = 0;
        }
    }
    constexpr static_vector(static_vector const& rhs) {
        for(size_t i = 0; i < CAPACITY; i++) {
            content_[i] = rhs[i];
        }
    }
    // For some reason, this triggers a circuit compilation error
    // ~static_vector() {
    //     size_ = 0;
    // }
    constexpr auto operator=(static_vector const& rhs) -> static_vector& {
        for(size_t i = 0; i < CAPACITY; i++) {
            content_[i] = rhs[i];
        }
        return *this;
    }
    constexpr auto data() -> Byte* {
        return &content_;
    }
    constexpr auto content() -> std::array<Byte, CAPACITY>& {
        return reinterpret_cast<std::array<Byte, CAPACITY>&>(content_);
    }
    constexpr auto operator[](std::size_t index) -> Byte& {
        return content_[index];
    }
    constexpr auto operator[](std::size_t index) const -> const Byte& {
        const Byte& retval = content_[index];
        return retval;
    }
    constexpr auto size() const -> std::size_t {
        return CAPACITY;
    }
    constexpr auto capacity() -> std::size_t {
        return CAPACITY;
    }
    bool operator==(const static_vector& rhs) const {
        for(size_t i = 0; i < CAPACITY; i++) {
            if(content_[i] != rhs[i]) {
                return false;
            }
        }
        return true;
    }
    bool operator!=(const static_vector& rhs) const {
        return !(*this == rhs);
    }
    constexpr auto begin() {
        return &content_[0];
    }
    constexpr auto end() {
        return &content_[0] + CAPACITY;
    }
    constexpr const auto begin() const {
        return &content_[0];
    }
    constexpr const auto end() const {
        return &content_[0] + CAPACITY;
    }
} __attribute__((packed));

using Bytes32 = static_vector<Byte, 32>;
using Bytes48 = static_vector<Byte, 48>;
using Bytes64 = static_vector<Byte, 64>;
using Bytes96 = static_vector<Byte, 96>;

[[circuit]] static_vector<Byte> qwe (static_vector<Byte> inp) {
    return inp;
}