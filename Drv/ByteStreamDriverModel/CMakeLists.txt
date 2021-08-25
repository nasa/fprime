####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/ByteStreamRecvPortAi.xml"
    "${CMAKE_CURRENT_LIST_DIR}/ByteStreamSendPortAi.xml"
    "${CMAKE_CURRENT_LIST_DIR}/ByteStreamPollPortAi.xml"
    "${CMAKE_CURRENT_LIST_DIR}/ByteStreamReadyPortAi.xml"
    "${CMAKE_CURRENT_LIST_DIR}/ByteStreamDriverComponentAi.xml"
)

register_fprime_module()
