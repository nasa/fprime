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
find_package(OpenSSL REQUIRED)

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/config/AESEncryptorConfig")

register_fprime_library(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/AESEncryptor.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/AESEncryptor.cpp"
   DEPENDS
       OpenSSL::Crypto
       Svc_Encryption_AESEncryptor_config_AESEncryptorConfig
)

### Unit Tests ###
register_fprime_ut(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/AESEncryptor.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/AESEncryptorTestMain.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/AESEncryptorTester.cpp"
    DEPENDS
        STest # For rules-based testing
       OpenSSL::Crypto
       Svc_Encryption_AESEncryptor_config_AESEncryptorConfig
    UT_AUTO_HELPERS
)
