####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####

set(PLATFORM_SOURCES)
if(FPRIME_USE_BAREMETAL_SCHEDULER)
  list(APPEND PLATFORM_SOURCES "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentBaremetalImpl.cpp")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "VxWorks")
  list(APPEND PLATFORM_SOURCES "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentVxWorksImpl.cpp")
else()
  list(APPEND PLATFORM_SOURCES "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentLinuxImpl.cpp")
endif()

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/FatalHandler.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentCommonImpl.cpp"
    ${PLATFORM_SOURCES}
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/FatalHandler.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentImpl.hpp"
  DEPENDS
    Os
    Fw_Logger
)
