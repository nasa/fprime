####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/Prm.fpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/ParamBuffer.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/PrmBuffer.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/PrmExternalTypes.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/PrmString.hpp"
)

