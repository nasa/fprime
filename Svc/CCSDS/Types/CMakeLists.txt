####
# FPrime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
# UT_SOURCE_FILES: list of source files for unit tests
#
# More information in the F´ CMake API documentation:
# https://fprime.jpl.nasa.gov/latest/documentation/reference/
####

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Types.fpp"
)

# Uncomment and add any modules that this module depends on, else
# they might not be available when cmake tries to build this module.
#
# Module names are derived from the path from the nearest project/library/framework
# root when not specifically overridden by the developer. i.e. The module defined by
# `Ref/SignalGen/CMakeLists.txt` will be named `Ref_SignalGen`.  `Ref/SignalGen`
# is an acceptable alternative and will be internally converted to `Ref_SignalGen`.
#
#set(MOD_DEPS
#  MyPackage_MyOtherModule
#)

register_fprime_module()
