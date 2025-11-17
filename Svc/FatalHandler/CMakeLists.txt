####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####

set(FATAL_HANDLER_SOURCES
  "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentCommonImpl.cpp"
)

if(FPRIME_USE_BAREMETAL_SCHEDULER)
  list(APPEND FATAL_HANDLER_SOURCES "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentBaremetalImpl.cpp")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "VxWorks") 
  list(APPEND FATAL_HANDLER_SOURCES "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentVxWorksImpl.cpp")
else()
  list(APPEND FATAL_HANDLER_SOURCES "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentLinuxImpl.cpp")
endif()

register_fprime_module(
  SOURCES
    ${FATAL_HANDLER_SOURCES}
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/FatalHandler.fpp"
  DEPENDS
    Os
    Fw_Logger
)
