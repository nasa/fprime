####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/GroundInterface.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/GroundInterface.cpp"
)
set(MOD_DEPS "Utils/Types")
register_fprime_module()

# Rules based unit testing
set(UT_MOD_DEPS
        STest
        Fw/Types)

set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/GroundInterface.fpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestMain.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/GroundInterfaceRules.cpp"
        )
# STest Includes for this UT
register_fprime_ut()
