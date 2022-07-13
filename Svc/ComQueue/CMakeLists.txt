####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(MOD_DEPS
        Utils/Types
        Fw/Buffer
        Fw/Types
        )
set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/ComQueue.fpp"
        "${CMAKE_CURRENT_LIST_DIR}/ComQueue.cpp"
        )
register_fprime_module()

### UTs ###
set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/ComQueue.fpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestMain.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
        )
register_fprime_ut()
