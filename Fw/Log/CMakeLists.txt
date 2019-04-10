####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/LogPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/LogTextPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/LogBuffer.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/LogPacket.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/LogString.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TextLogString.cpp"

)
register_fprime_module()

# Add unit test directory
## add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/ut")
