include(CheckCSourceCompiles)
include(CheckCXXSourceCompiles)

set(AVX_CODE "
  #include <immintrin.h>

  int main()
  {
    __m256 a;
    a = _mm256_set1_ps(0);
    return 0;
  }
")

set(AVX2_CODE "
  #include <immintrin.h>

  int main()
  {
    __m256i a = {0};
    a = _mm256_abs_epi16(a);
    __m256i x;
    _mm256_extract_epi64(x, 0); // we rely on this in our AVX2 code
    return 0;
  }
")

macro(check_avx_lang lang type flags)
    set(__FLAG_I 1)
    set(CMAKE_REQUIRED_FLAGS_SAVE ${CMAKE_REQUIRED_FLAGS})
    foreach(__FLAG ${flags})
        if(NOT ${lang}_${type}_FOUND)
            set(CMAKE_REQUIRED_FLAGS ${__FLAG})
            if(lang STREQUAL "CXX")
                check_cxx_source_compiles("${${type}_CODE}" ${lang}_HAS_${type}_${__FLAG_I})
            else()
                check_c_source_compiles("${${type}_CODE}" ${lang}_HAS_${type}_${__FLAG_I})
            endif()
            if(${lang}_HAS_${type}_${__FLAG_I})
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

endmacro()

macro(check_avx)
    check_avx_lang(C "AVX" " ;-mavx;/arch:AVX")
    check_avx_lang(C "AVX2" " ;-mavx2 -mfma;/arch:AVX2")

    check_avx_lang(CXX "AVX" " ;-mavx;/arch:AVX")
    check_avx_lang(CXX "AVX2" " ;-mavx2 -mfma;/arch:AVX2")
endmacro()