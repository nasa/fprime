
####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/CmdDispatcher.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/CommandDispatcherImpl.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/CommandDispatcher.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/CommandDispatcherImpl.hpp"
)
### UTs ###
# Run the UTs with each setting of CmdDispatcherCfg::ExecuteCommandWhenSequenceTrackerTableIsFull
foreach (EXECUTE_CMD IN ITEMS false true)
    register_fprime_ut(
        CmdDispatcherTest_ExecuteWhenFull_${EXECUTE_CMD}
      AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/CmdDispatcher.fpp"
      SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/CommandDispatcherImpl.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/CommandDispatcherTestMain.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/CommandDispatcherTester.cpp"
    )
    if (TARGET CmdDispatcherTest_ExecuteWhenFull_${EXECUTE_CMD})
        target_compile_definitions(CmdDispatcherTest_ExecuteWhenFull_${EXECUTE_CMD}
            PRIVATE CMD_DISPATCHER_EXECUTE_WHEN_TRACKER_FULL=${EXECUTE_CMD})
    endif()
endforeach()
