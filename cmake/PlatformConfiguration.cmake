include(CheckTypeSize)

function(define_target_architecture prefix_name target_architecture)
    string(TOUPPER ${target_architecture} UPPER_TARGET_ARCHITECTURE)
    string(TOUPPER ${prefix_name} UPPER_PREFIX_NAME)

    add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_ARCHITECTURE_IS_${UPPER_TARGET_ARCHITECTURE})
endfunction()

function(define_target_endianness prefix_name target_architecture)
    string(TOUPPER ${target_architecture} UPPER_TARGET_ARCHITECTURE)
    string(TOUPPER ${prefix_name} UPPER_PREFIX_NAME)

    if(${UPPER_TARGET_ARCHITECTURE} STREQUAL "X86" OR
       ${UPPER_TARGET_ARCHITECTURE} STREQUAL "X86_64")
        add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_CPU_IS_X86_FAMILY)
    elseif(${UPPER_TARGET_ARCHITECTURE} STREQUAL "ARMV8" OR
           ${UPPER_TARGET_ARCHITECTURE} STREQUAL "ARM64" OR
           ${UPPER_TARGET_ARCHITECTURE} STREQUAL "ARMV7S" OR
           ${UPPER_TARGET_ARCHITECTURE} STREQUAL "ARMV6" OR
           ${UPPER_TARGET_ARCHITECTURE} STREQUAL "ARMV5" OR
           ${UPPER_TARGET_ARCHITECTURE} STREQUAL "ARM32" OR
           ${UPPER_TARGET_ARCHITECTURE} STREQUAL "ARMV7")
        add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_CPU_IS_ARM_FAMILY)
    elseif(${UPPER_TARGET_ARCHITECTURE} STREQUAL "PPC" OR
           ${UPPER_TARGET_ARCHITECTURE} STREQUAL "PPC64")
        add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_CPU_IS_PPC_FAMILY)
    endif()

    if(${UPPER_TARGET_ARCHITECTURE} STREQUAL "ALPHA" OR
       ${UPPER_TARGET_ARCHITECTURE} STREQUAL "ARMV8" OR
       ${UPPER_TARGET_ARCHITECTURE} STREQUAL "ARM64" OR
       ${UPPER_TARGET_ARCHITECTURE} STREQUAL "ARMV7S" OR
       ${UPPER_TARGET_ARCHITECTURE} STREQUAL "ARMV6" OR
       ${UPPER_TARGET_ARCHITECTURE} STREQUAL "ARMV5" OR
       ${UPPER_TARGET_ARCHITECTURE} STREQUAL "ARM32" OR
       ${UPPER_TARGET_ARCHITECTURE} STREQUAL "ARMV7" OR
       ${UPPER_TARGET_ARCHITECTURE} STREQUAL "X86" OR
       ${UPPER_TARGET_ARCHITECTURE} STREQUAL "X86_64")
        add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_CPU_IS_LITTLE_ENDIAN)
    elseif(${UPPER_TARGET_ARCHITECTURE} STREQUAL "M68K" OR
           ${UPPER_TARGET_ARCHITECTURE} STREQUAL "S390" OR
           ${UPPER_TARGET_ARCHITECTURE} STREQUAL "S390X" OR
           ${UPPER_TARGET_ARCHITECTURE} STREQUAL "SPARC32" OR
           ${UPPER_TARGET_ARCHITECTURE} STREQUAL "SPARC64" OR
           ${UPPER_TARGET_ARCHITECTURE} STREQUAL "PPC" OR
           ${UPPER_TARGET_ARCHITECTURE} STREQUAL "PPC64")
        add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_CPU_IS_BIG_ENDIAN)
    endif()
endfunction()

function(define_target_platform_size prefix_name target_architecture)
    string(TOUPPER ${target_architecture} UPPER_TARGET_ARCHITECTURE)
    string(TOUPPER ${prefix_name} UPPER_PREFIX_NAME)

    if(${CMAKE_TARGET_ARCHITECTURE} STREQUAL ${CMAKE_HOST_SYSTEM_PROCESSOR})
        #        check_type_size(int CMAKE_TARGET_INTEGER_SIZE_BYTE)
        #        math(EXPR CMAKE_TARGET_INTEGER_SIZE_BITS "${CMAKE_TARGET_INTEGER_SIZE_BYTE} * 8")
        #        add_definitions(-D${UPPER_PREFIX_NAME}_MP_WORD_BITS=${CMAKE_TARGET_INTEGER_SIZE_BITS})

        if(${UPPER_TARGET_ARCHITECTURE} STREQUAL "ALPHA" OR
           ${UPPER_TARGET_ARCHITECTURE} STREQUAL "ARM64" OR
           ${UPPER_TARGET_ARCHITECTURE} STREQUAL "X86_64" OR
           ${UPPER_TARGET_ARCHITECTURE} STREQUAL "SPARC64" OR
           ${UPPER_TARGET_ARCHITECTURE} STREQUAL "PPC64" OR
           ${UPPER_TARGET_ARCHITECTURE} STREQUAL "S390X")
            add_definitions(-D${UPPER_PREFIX_NAME}_MP_WORD_BITS=64)
            add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_CPU_HAS_NATIVE_64BIT)
        else()
            add_definitions(-D${UPPER_PREFIX_NAME}_MP_WORD_BITS=32)
        endif()
    else()
        if(${UPPER_TARGET_ARCHITECTURE} STREQUAL "ALPHA" OR
           ${UPPER_TARGET_ARCHITECTURE} STREQUAL "ARM64" OR
           ${UPPER_TARGET_ARCHITECTURE} STREQUAL "X86_64" OR
           ${UPPER_TARGET_ARCHITECTURE} STREQUAL "SPARC64" OR
           ${UPPER_TARGET_ARCHITECTURE} STREQUAL "PPC64" OR
           ${UPPER_TARGET_ARCHITECTURE} STREQUAL "S390X")
            add_definitions(-D${UPPER_PREFIX_NAME}_MP_WORD_BITS=64)
            add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_CPU_HAS_NATIVE_64BIT)
        else()
            add_definitions(-D${UPPER_PREFIX_NAME}_MP_WORD_BITS=32)
        endif()
    endif()

    if(${UPPER_TARGET_ARCHITECTURE} STREQUAL "X86_64")
        add_definitions(-D${UPPER_PREFIX_NAME}_MP_USE_X86_64_ASM)
    elseif(${UPPER_TARGET_ARCHITECTURE} STREQUAL "X86")
        add_definitions(-D${UPPER_PREFIX_NAME}_MP_USE_X86_32_ASM)
    endif()
endfunction()

function(define_target_platform_features prefix_name)
    include(CheckSymbolExists)

    string(TOUPPER ${prefix_name} UPPER_PREFIX_NAME)

    if(UNIX)
        if(APPLE)
            add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_OS_IS_DARWIN)

            add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_OS_HAS_ARC4RANDOM)
            add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_OS_HAS_MEMSET_S)
        else()
            add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_OS_HAS_CLOCK_GETTIME)
            add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_OS_HAS_POSIX_MLOCK)
        endif()

        check_symbol_exists(clock_gettime sys/time.h HAVE_CLOCK_GETTIME)
        check_symbol_exists(gettimeofday sys/time.h HAVE_GETTIMEOFDAY)

        if(HAVE_GETTIMEOFDAY)
            add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_OS_HAS_GETTIMEOFDAY)
        endif()

        add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_OS_TYPE_IS_UNIX)

        add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_OS_HAS_GMTIME_R)
        add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_OS_HAS_DEV_RANDOM)
        add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_OS_HAS_POSIX1)
        add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_OS_HAS_FILELSYSTEM)
        add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_OS_HAS_TIMEGM)
        add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_OS_HAS_SOCKETS)
        add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_OS_HAS_READDIR)
        add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_OS_HAS_DLOPEN)
    elseif(WIN32)
        add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_OS_TYPE_IS_WINDOWS)
        add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_OS_HAS_WIN32)

        add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_OS_HAS_RTLGENRANDOM)
        add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_OS_HAS_GMTIME_S)
        add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_OS_HAS_LOADLIBRARY)
        add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_OS_HAS_MKGMTIME)
        add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_OS_HAS_QUERY_PREF_COUNTER)
        add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_OS_HAS_VIRTUAL_LOCK)
        add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_OS_HAS_RTLSECUREZEROMEMORY)
        add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_OS_HAS_STL_FILESYSTEM_MSVC)
    endif()

    add_definitions(-D${UPPER_PREFIX_NAME}_TARGET_OS_HAS_THREADS)
endfunction()