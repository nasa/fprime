####
# Sets a list of source files for cmake to process as part of autocoding.
####
register_fprime_config(
       FppToCppSettings
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/ToCpp.fpp"
    INTERFACE
)
