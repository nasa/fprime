####
# Darwin.cmake:
#
# Darwin based platform file used for Darwin (Mac OS X) targets. Note: this sets some OS X flags before calling into the common
# Linux implementations to use the posix types defined there.
####
include("${FPRIME_FRAMEWORK_PATH}/cmake/platform/Darwin.cmake")
register_fprime_config(
        Darwin_Special_Config
    INTERFACE
    CHOOSES_IMPLEMENTATIONS
        Test_Implementation_Platform
        Test_Override_Unused
)
