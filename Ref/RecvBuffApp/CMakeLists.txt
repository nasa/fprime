####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/RecvBuffComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/PacketStatSerializableAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/RecvBuffComponentImpl.cpp"
)
set(MOD_DEPS "")
# For shared libraries, Os must exist in the module list
if (BUILD_SHARED_LIBS)
    list(APPEND MOD_DEPS "Fw/Cmd")
endif()
register_fprime_module()
