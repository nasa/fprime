####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####

# Basic module dependencies
set(MOD_DEPS
  ${CMAKE_THREAD_LIBS_INIT}
  Fw/Cfg
  Fw/Types
  Fw/Logger
  Utils/Hash
)

# Basic source files used in every OSAL layer.  Contains common code and helpers.
set(SOURCE_FILES
    # Common for tasks
    "${CMAKE_CURRENT_LIST_DIR}/Common/TaskString.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/TaskCommon.cpp"

    # Common for queues
    "${CMAKE_CURRENT_LIST_DIR}/Common/QueueCommon.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/QueueString.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/SimpleQueueRegistry.cpp"

    # Common for files
    "${CMAKE_CURRENT_LIST_DIR}/Common/ValidateFileCommon.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/ValidatedFile.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/FileCommon.cpp"

    # Common for interval timer
    "${CMAKE_CURRENT_LIST_DIR}/Common/IntervalTimerCommon.cpp"
)
if (NOT FPRIME_DISABLE_DEFAULT_LOGGER)
    list(APPEND SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/Common/LogDefault.cpp")
endif()

require_fprime_implementation(Os_Task)
require_fprime_implementation(Os_Queue)
require_fprime_implementation(Os_File)
require_fprime_implementation(Os_Timer)
require_fprime_implementation(Os_Resource)
require_fprime_implementation(Os_Log)
require_fprime_implementation(Os_Mutex)
require_fprime_implementation(Os_InterruptLock)
require_fprime_implementation(Os_Watchdog)

register_fprime_module()

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Posix")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Linux")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Darwin")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Stubs")

### UTS ### Note: 3 separate UTs registered here.
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/OsQueueTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/IntervalTimerTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/OsValidateFileTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/OsTaskTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/OsFileSystemTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/OsSystemResourcesTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/OsMutexBasicLockableTest.cpp"
)
register_fprime_ut()


