#---------------------------------------------------------------------------#
# Copyright (c) 2018-2020 Mikhail Komarov <nemo@nil.foundation>
#
# Distributed under the Boost Software License, Version 1.0
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt
#---------------------------------------------------------------------------#

include(CheckCSourceCompiles)
include(CheckCXXSourceCompiles)
include(CheckCSourceRuns)
include(CheckCXXSourceRuns)

set(AVX_CODE "
#include <immintrin.h>
int main()
{
    __m256 a, b, c;
    const float src[8] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f };
    float dst[8];
    a = _mm256_loadu_ps( src );
    b = _mm256_loadu_ps( src );
    c = _mm256_add_ps( a, b );
    _mm256_storeu_ps( dst, c );

    for( int i = 0; i < 8; i++ ){
        if( ( src[i] + src[i] ) != dst[i] ){
            return -1;
        }
    }
    return 0;
}
")

set(AVX2_CODE "
#include <immintrin.h>

int main()
{
    __m256i a, b, c;
    const int src[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    int dst[8];
    a =  _mm256_loadu_si256( (__m256i*)src );
    b =  _mm256_loadu_si256( (__m256i*)src );
    c = _mm256_add_epi32( a, b );
    _mm256_storeu_si256( (__m256i*)dst, c );

    for( int i = 0; i < 8; i++ ){
    if( ( src[i] + src[i] ) != dst[i] ){
        return -1;
    }
    }

    return 0;
}

")

set(AVX512_CODE "
#include <immintrin.h>

int main()
{
    __m512i a = {0};
    __m512i b = _mm512_set_epi64(1, 1, 1, 1, 1, 1, 1, 1);
    __m512i c = _mm256_add_ps( a, b );
    for( int i = 0; i < 8; i++ ){
        if( ( a[i] + b[i] ) != c[i] ){
            return -1;
        }
    }
    return 0;
}
")

macro(check_avx_lang lang type flags)
    set(__FLAG_I 1)
    set(CMAKE_REQUIRED_FLAGS_SAVE ${CMAKE_REQUIRED_FLAGS})
    foreach(__FLAG ${flags})
        if(NOT ${lang}_${type}_FOUND)
            set(CMAKE_REQUIRED_FLAGS ${__FLAG})
            check_c_source_runs("${${type}_CODE}" HAS_${type}_${__FLAG_I})
            check_cxx_source_compiles("${${type}_CODE}" COMPPILES)
            message(STATUS "CXXC=${COMPPILES}")
            if(HAS_${type}_${__FLAG_I})
                set(${lang}_${type}_FOUND TRUE CACHE BOOL "${lang} ${type} support")
                set(${lang}_${type}_FLAGS "${__FLAG}" CACHE STRING "${lang} ${type} flags")
            endif()
            math(EXPR __FLAG_I "${__FLAG_I}+1")
        endif()
    endforeach()
    set(CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS_SAVE})

    if(NOT ${lang}_${type}_FOUND)
        set(${lang}_${type}_FOUND FALSE CACHE BOOL "${lang} ${type} support")
        set(${lang}_${type}_FLAGS "" CACHE STRING "${lang} ${type} flags")
    endif()

    mark_as_advanced(${lang}_${type}_FOUND ${lang}_${type}_FLAGS)
    message(STATUS "${lang}_${type}_FOUND=${${lang}_${type}_FOUND}")

endmacro()

macro(check_avx)
    check_avx_lang(CXX "AVX" "-mavx;/arch:AVX")
    check_avx_lang(CXX "AVX2" "-mavx2 -mfma;/arch:AVX2")
    check_avx_lang(CXX "AVX512" "-mavx512f -mfma;/arch:AVX512")
endmacro()
