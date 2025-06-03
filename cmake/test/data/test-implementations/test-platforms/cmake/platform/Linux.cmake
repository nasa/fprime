####
# Linux.cmake:
#
# Linux platform file for standard linux targets. Merely defers to ./Linux.cmake.
####
include("${FPRIME_FRAMEWORK_PATH}/cmake/platform/Linux.cmake")
register_fprime_config(
        Linux_Special_Config
    INTERFACE
    CHOOSES_IMPLEMENTATIONS
        Test_Implementation_Platform
        Test_Override_Unused
)

