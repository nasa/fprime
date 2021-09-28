####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Deframer.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/DeframerComponentImpl.cpp"
)

set(MOD_DEPS
    Svc/FramingProtocol
    Utils/Types
)

register_fprime_module()

#### UTS ###

# TODO: DISABLED - the autocoder breaks attempting to generate the UT autocoded test files for both test cases
# re-enable when cmake ut autocoder hooks are fixed
# set(UT_SOURCE_FILES
#   "${CMAKE_CURRENT_LIST_DIR}/Deframer.fpp"
#   "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
#   "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestMain.cpp"
# )
# register_fprime_ut()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Deframer.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut-unified/DeframerRules.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut-unified/Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut-unified/TestMain.cpp"
)
set(UT_MOD_DEPS STest)
register_fprime_ut(Svc_Deframer_with_fprime)
