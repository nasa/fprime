register_fprime_library(
        TestRelative
    AUTOCODER_INPUTS
        ../constant1.fpp
        ./constant2.fpp
    SOURCES
        ./uses_types.cpp
)

# Record what CMake will reconfigure for. Rewritten on every configure, so the
# test suite can both read the list and use the file as a witness that a
# reconfigure happened.
get_property(TEST_CONFIGURE_DEPENDS DIRECTORY PROPERTY CMAKE_CONFIGURE_DEPENDS)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/configure-depends.txt" "${TEST_CONFIGURE_DEPENDS}")