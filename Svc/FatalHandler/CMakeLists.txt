####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####

if(FPRIME_USE_BAREMETAL_SCHEDULER)
  register_fprime_module(
    AUTOCODER_INPUTS
      "${CMAKE_CURRENT_LIST_DIR}/FatalHandler.fpp"
    SOURCES
      "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentCommonImpl.cpp"
      "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentBaremetalImpl.cpp"
    HEADERS
      "${CMAKE_CURRENT_LIST_DIR}/FatalHandler.hpp"
      "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentImpl.hpp"
    DEPENDS
      Os
      Fw_Logger
  )
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "VxWorks")
  register_fprime_module(
    AUTOCODER_INPUTS
      "${CMAKE_CURRENT_LIST_DIR}/FatalHandler.fpp"
    SOURCES
      "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentCommonImpl.cpp"
      "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentVxWorksImpl.cpp"
    HEADERS
      "${CMAKE_CURRENT_LIST_DIR}/FatalHandler.hpp"
      "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentImpl.hpp"
    DEPENDS
      Os
      Fw_Logger
  )
else()
  register_fprime_module(
    AUTOCODER_INPUTS
      "${CMAKE_CURRENT_LIST_DIR}/FatalHandler.fpp"
    SOURCES
      "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentCommonImpl.cpp"
      "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentLinuxImpl.cpp"
    HEADERS
      "${CMAKE_CURRENT_LIST_DIR}/FatalHandler.hpp"
      "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentImpl.hpp"
    DEPENDS
      Os
      Fw_Logger
  )
endif()
