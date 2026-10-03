####
# F Prime CMakeLists.txt:
#
# SOURCES: list of source files (to be compiled)
# AUTOCODER_INPUTS: list of files to be passed to the autocoders
# DEPENDS: list of libraries that this module depends on
#
# More information in the F´ CMake API documentation:
# https://fprime.jpl.nasa.gov/devel/docs/reference/api/cmake/API/
#
####

# Shared AES-256-GCM cipher for the SDLS encryptor and decryptor - only added when OpenSSL is available
find_package(OpenSSL 3.5)

if (NOT OPENSSL_FOUND)
    message(STATUS "Unable to add Svc_Ccsds_Utils_AesGcm. OpenSSL 3.5 not found")
    return()
endif()

register_fprime_library(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/AesGcmCipher.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/AesGcmCipher.hpp"
  DEPENDS
    Svc_Ccsds_Types
    OpenSSL::Crypto
)

### Unit Tests ###
register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/AesGcmCipherTest.cpp"
  DEPENDS
    Svc_Ccsds_Utils_AesGcm
    OpenSSL::Crypto
)
