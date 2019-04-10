####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/TlmPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/TlmBuffer.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TlmPacket.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TlmString.cpp"
)
register_fprime_module()

# Add unit test directory
## TODO-MSTARCH add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/ut")
