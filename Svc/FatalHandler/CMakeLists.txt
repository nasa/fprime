####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####

if(FPRIME_USE_BAREMETAL_SCHEDULER)
  list(APPEND SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentBaremetalImpl.cpp")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "VxWorks") 
  list(APPEND SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentVxWorksImpl.cpp")
else()
  list(APPEND SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentLinuxImpl.cpp")
endif()

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/FatalHandler.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentCommonImpl.cpp"
  DEPENDS
    Os
    Fw/Logger
)
