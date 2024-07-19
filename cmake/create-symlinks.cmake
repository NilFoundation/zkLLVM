message("Creating symlinks")

list(APPEND BINARIES assigner transpiler)
list(APPEND LLVM_BINARIES clang llvm-link)

# Link directory would differ for cases when we have DESTDIR from CPack or simple installation
if (DEFINED ENV{DESTDIR})
    set(LINK_DESTDIR $ENV{DESTDIR}/var/empty/bin)
else()
    set(LINK_DESTDIR ${CMAKE_INSTALL_PREFIX}/bin)
endif()

file(MAKE_DIRECTORY ${LINK_DESTDIR})
message("LINK_DESTDIR ${LINK_DESTDIR}")

foreach(BINARY IN LISTS BINARIES)
    set(LINK_NAME ${LINK_DESTDIR}/${BINARY})
    set(LINK_TARGET "../lib/zkllvm/bin/${BINARY}")
    message("${LINK_NAME} -> ${LINK_TARGET}")
    file(CREATE_LINK ${LINK_TARGET} ${LINK_NAME} SYMBOLIC)
endforeach()

foreach(BINARY IN LISTS LLVM_BINARIES)
    set(LINK_NAME ${LINK_DESTDIR}/${BINARY}-zkllvm)
    set(LINK_TARGET "../lib/zkllvm/bin/${BINARY}")
    message("${LINK_NAME} -> ${LINK_TARGET}")
    file(CREATE_LINK ${LINK_TARGET} ${LINK_NAME} SYMBOLIC)
endforeach()
