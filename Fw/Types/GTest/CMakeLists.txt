####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
if(BUILD_TESTING)
    register_fprime_module(
      SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/Bytes.cpp"
      HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/Bytes.hpp"
      DEPENDS
        gtest
        Fw_Types
        Fw_Comp
        Fw_Port
    )
endif()
