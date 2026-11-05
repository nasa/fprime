####
# F Prime CMakeLists.txt:
#
# SOURCES: list of source files (to be compiled)
# AUTOCODER_INPUTS: list of files to be passed to the autocoders
# DEPENDS: list of libraries that this module depends on
#
# More information in the F´ CMake API documentation:
# https://fprime.jpl.nasa.gov/latest/docs/reference/api/cmake/API/
#
####

# Module names are derived from the path from the nearest project/library/framework
# root when not specifically overridden by the developer, i.e. the module defined by
# `MyProj/Some/Path/CMakeLists.txt` will be named `MyProj_Some_Path`.
# AesGcmEncryptor requires OpenSSL - only add if available and usable by the active toolchain
find_package(OpenSSL 3.0)

if (OPENSSL_FOUND)
    include(CheckCXXSourceCompiles)
    set(CMAKE_REQUIRED_LIBRARIES OpenSSL::Crypto)
    check_cxx_source_compiles("
        #include <openssl/evp.h>
        int main() { EVP_CIPHER_CTX_free(EVP_CIPHER_CTX_new()); return 0; }"
        FPRIME_OPENSSL_CRYPTO_USABLE
    )
    unset(CMAKE_REQUIRED_LIBRARIES)
endif()

if (NOT OPENSSL_FOUND OR NOT FPRIME_OPENSSL_CRYPTO_USABLE)
    message(STATUS "Unable to add Svc_Ccsds_AesGcmEncryptor. OpenSSL 3.0 not found for the target toolchain")
    return()
endif()

register_fprime_library(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/AesGcmEncryptor.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/AesGcmEncryptor.cpp"
   DEPENDS
       OpenSSL::Crypto
)

### Unit Tests ###
register_fprime_ut(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/AesGcmEncryptor.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/AesGcmEncryptorTestMain.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/AesGcmEncryptorTester.cpp"
    DEPENDS
        STest # For rules-based testing
       OpenSSL::Crypto
    UT_AUTO_HELPERS
)
