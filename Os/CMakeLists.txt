####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Models")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Stub")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Posix")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Generic")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Linux")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Darwin")

set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/file/SyntheticFileSystem.cpp"
)
set(MOD_DEPS config)
register_fprime_module(Os_Test_File_SyntheticFileSystem)


# Basic module dependencies
set(MOD_DEPS
  ${CMAKE_THREAD_LIBS_INIT}
  Fw/Cfg
  Fw/Types
  Fw/Logger
  Utils/Hash
)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    list(APPEND MOD_DEPS -lrt)
endif()

# Basic source files used in every OSAL layer.  Contains common code and helpers.
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/IntervalTimerCommon.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Posix/IntervalTimer.cpp"

    "${CMAKE_CURRENT_LIST_DIR}/ValidateFileCommon.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ValidatedFile.cpp"

    # Refactored common files
    "${CMAKE_CURRENT_LIST_DIR}/Os.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Console.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/File.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Task.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Mutex.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/FileSystem.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Directory.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Condition.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Queue.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Cpu.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Memory.cpp"
)
# Check for default logger
if (NOT FPRIME_DISABLE_DEFAULT_LOGGER)
    list(APPEND SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/LogDefault.cpp"
    )
endif()
register_fprime_module()

require_fprime_implementation(Os/File)
require_fprime_implementation(Os/FileSystem)
require_fprime_implementation(Os/Directory)
require_fprime_implementation(Os/Mutex)
#require_fprime_implementation(Os/Task)
require_fprime_implementation(Os/Console)
require_fprime_implementation(Os/Queue)
require_fprime_implementation(Os/Cpu)
require_fprime_implementation(Os/Memory)


### UTS ### Note: 3 separate UTs registered here.
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/OsTestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/IntervalTimerTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/OsValidateFileTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/OsMutexBasicLockableTest.cpp"
)
register_fprime_ut()
if (BUILD_TESTING)
    foreach (TEST IN ITEMS 
                StubFileTest PosixFileTest
                StubMutexTest PosixMutexTest
                StubDirectoryTest PosixDirectory
                StubFileSystemTest PosixFileSystemTest
            ) # add? : StubTaskTest PosixTaskTest
        if (TARGET "${TEST}")
            add_dependencies("Os_ut_exe" "${TEST}")
        endif()
    endforeach()
endif()
