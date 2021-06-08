####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
set(MOD_DEPS Os)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/LinuxSerialDriverComponentAi.xml"
        "${CMAKE_CURRENT_LIST_DIR}/LinuxSerialDriverComponentImplCommon.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/LinuxSerialDriverComponentImpl.cpp"
    )
    register_fprime_module()
else()
    message(STATUS "Cannot use ${CMAKE_CURRENT_LIST_DIR} with platform ${CMAKE_SYSTEM_NAME}. Skipping.")
endif()

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
