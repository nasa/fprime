####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/DeframingProtocol.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/FramingProtocol.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/FprimeProtocol.cpp"
  DEPENDS
    Fw/Buffer
    Fw/Time
    Utils/Hash
    Utils/Types
)

register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/DeframingTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/FramingTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/main.cpp"
  DEPENDS
    STest
)
