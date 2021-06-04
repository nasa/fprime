####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "arm-linux-gnueabihf" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/LinuxSerialDriverComponentAi.xml"
        "${CMAKE_CURRENT_LIST_DIR}/LinuxSerialDriverComponentImplCommon.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/LinuxSerialDriverComponentImpl.cpp"
    )
else()
    message(FATAL_ERROR "Cannot use ${CMAKE_CURRENT_LIST_DIR} with platform ${CMAKE_SYSTEM_NAME}.")
endif()

set(MOD_DEPS
    Os
)

register_fprime_module()

### UTs ###
# Note: this UT expects user input
#set(UT_MOD_DEPS
#    Os
#)
#set(UT_SOURCE_FILES
#  "${CMAKE_CURRENT_LIST_DIR}/LinuxSerialDriverComponentAi.xml"
#  "${CMAKE_CURRENT_LIST_DIR}/test/ut/main.cpp"
#  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
#)
#register_fprime_ut()
