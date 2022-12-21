####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Deframer.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/Deframer.cpp"
)

set(MOD_DEPS
    Svc/FramingProtocol
    Utils/Types
)

register_fprime_module()

#### UTS ###

# These tests verify the Deframer component against a mock
# deframing protocol
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Deframer.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestMain.cpp"
)
register_fprime_ut()

# These tests verify the Deframer component against the
# F Prime deframing protocol located at Svc/FramingProtocol.
# They also verify the deframing part of the F Prime protocol
# implementation.
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Deframer.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut-fprime-protocol/GenerateFrames.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut-fprime-protocol/SendBuffer.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut-fprime-protocol/Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut-fprime-protocol/TestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut-fprime-protocol/UplinkFrame.cpp"
)
set(UT_MOD_DEPS STest)
register_fprime_ut(Svc_Deframer_fprime_protocol)
