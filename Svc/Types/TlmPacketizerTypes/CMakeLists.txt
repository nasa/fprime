####
# CMakeLists.txt:
#
# Sets up the fprime module build within CMake.
####
register_fprime_module(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/TlmPacketizerTypes.fpp"
    DEPENDS
        Fw_Types
)
