####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
if(CMAKE_BUILD_TYPE STREQUAL "TESTING")
    set(SOURCE_FILES
      "${CMAKE_CURRENT_LIST_DIR}/Bytes.cpp"
    )
    set(MOD_DEPS
      gtest
      Fw/Cfg
      Fw/Types
      Fw/Comp
      Fw/Port
    )
    register_fprime_module()
endif()
