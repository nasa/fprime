####
# Darwin.cmake:
#
# Darwin based platform file used for Darwin (Mac OS X) targets. Note: this sets some OS X flags before calling into the common
# Linux implementations to use the posix types defined there.
####
include("${FPRIME_FRAMEWORK_PATH}/cmake/platform/Darwin.cmake")
choose_fprime_implementation(Test/Implementation Test/Implementation/Platform)
choose_fprime_implementation(Test/Override Test/Implementation/Unused)
