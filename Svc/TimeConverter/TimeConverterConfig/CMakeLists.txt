####
# Svc/TimeConverter/TimeConverterConfig/CMakeLists.txt:
#
# Compile-time configuration for the TimeConverter component.
####
register_fprime_config(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/TimeConverterCfg.fpp"
    DEPENDS
        "${FPRIME_GLOBAL_INTERFACE_TARGET}"
)
