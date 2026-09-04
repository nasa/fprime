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
# AesGcmEncryptor requires OpenSSL - only add if available
find_package(OpenSSL)

if (NOT OPENSSL_FOUND)
    message(STATUS "Unable to add Svc_Ccsds_AesGcmEncryptor. OpenSSL not found")
    return()
endif()

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/config/AesGcmEncryptorConfig")

register_fprime_library(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/AesGcmEncryptor.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/AesGcmEncryptor.cpp"
   DEPENDS
       OpenSSL::Crypto
       Svc_Ccsds_AesGcmEncryptor_config_AesGcmEncryptorConfig
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
       Svc_Ccsds_AesGcmEncryptor_config_AesGcmEncryptorConfig
    UT_AUTO_HELPERS
)
