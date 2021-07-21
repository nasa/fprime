####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentCommonImpl.cpp"
)

message(STATUS "${CMAKE_SYSTEM_NAME} System}")

if(CMAKE_SYSTEM_NAME STREQUAL "VxWorks")
  list(APPEND SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentVxWorksImpl.cpp"
  )
else()
  list(APPEND SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/FatalHandlerComponentLinuxImpl.cpp"
  )
endif()


set(MOD_DEPS
  Os
  Fw/Logger
)
register_fprime_module()
