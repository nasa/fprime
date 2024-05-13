####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
# UT_SOURCE_FILES: list of source files for unit tests
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/SeqDispatcher.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/SeqDispatcher.cpp"
)

# Uncomment and add any modules that this component depends on, else
# they might not be available when cmake tries to build this component.

set(MOD_DEPS
    common/types
    Svc/CmdSequencer
)

register_fprime_module()

# Register the unit test build
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/SeqDispatcherCommands.fppi"
  "${CMAKE_CURRENT_LIST_DIR}/SeqDispatcherTelemetry.fppi"
  "${CMAKE_CURRENT_LIST_DIR}/SeqDispatcherEvents.fppi"
  "${CMAKE_CURRENT_LIST_DIR}/SeqDispatcher.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestMain.cpp"
)
set(UT_AUTO_HELPERS ON)
register_fprime_ut()