####
# Linux.cmake:
#
# Linux platform file for standard linux targets. Merely defers to [./Linux-common.cmake](Linux-common.cmake).
####
include("${FPRIME_FRAMEWORK_PATH}/cmake/platform/Linux.cmake")
choose_fprime_implementation(Test/Implementation Test/Implementation/Platform)
choose_fprime_implementation(Test/Override Test/Implementation/Unused)

