####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
restrict_platforms(Posix)
add_custom_target("${FPRIME_CURRENT_MODULE}")

# -----------------------------------------
## Shared Section
# -----------------------------------------

if (BUILD_TESTING)
    add_compile_options(
        -Wno-conversion
    )
endif()

register_fprime_module(
    Os_Posix_Shared
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/error.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/error.hpp"
  DEPENDS
    Fw_Time
    Fw_Types
)

# Set up Posix implementations
register_os_implementation("File;FileSystem;Directory" Posix Os_Posix_Shared)
register_os_implementation("Console" Posix)

register_os_implementation("Task" Posix Os_Posix_Shared Fw_Time)
register_os_implementation("Mutex;ConditionVariable" Posix Os_Posix_Shared)
register_os_implementation("RawTime" Posix Os_Posix_Shared)

# -----------------------------------------
### Os/File/Posix Test Section
# -----------------------------------------
register_fprime_ut(PosixFileTest 
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/file/CommonTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/file/FileRules.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixFileTests.cpp"     
  DEPENDS
    Fw_Types
    Os_Test_File_SyntheticFileSystem
    STest
  CHOOSES_IMPLEMENTATIONS
    "Os_File_Posix" 
)

# -----------------------------------------
### Os/Console/Posix Test Section
# -----------------------------------------
register_fprime_ut(
     PosixConsoleTest 
  SOURCES
     "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixConsoleTests.cpp"
  DEPENDS
    Fw_Types
    STest
  CHOOSES_IMPLEMENTATIONS
    "Os_Console_Posix"
)

# -----------------------------------------
### Os/Task/Posix Test Section
# -----------------------------------------
register_fprime_ut(
     PosixTaskTest 
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/task/CommonTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/task/TaskRules.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixTaskTests.cpp"   
  DEPENDS
    Fw_Types
    STest
  CHOOSES_IMPLEMENTATIONS
     "Os_Task_Posix"
)

# -----------------------------------------
### Os/Mutex/Posix Test Section
# -----------------------------------------

register_fprime_ut(
     PosixMutexTest 
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/mutex/CommonTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/mutex/MutexRules.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixMutexTests.cpp"
  DEPENDS
    Fw_Types
    STest
  CHOOSES_IMPLEMENTATIONS
    "Os_Mutex_Posix"
)

# -----------------------------------------
### Os/FileSystem/Posix Test Section
# -----------------------------------------
register_fprime_ut(
     PosixFileSystemTest 
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/filesystem/CommonTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/filesystem/FileSystemRules.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixFileSystemTests.cpp"     
  DEPENDS
    Fw_Types
    STest
  CHOOSES_IMPLEMENTATIONS
    "Os_File_Posix"
)

# -----------------------------------------
### Os/Directory/Posix Test Section
# -----------------------------------------

register_fprime_ut(
     PosixDirectoryTest 
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/directory/CommonTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/directory/DirectoryRules.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixDirectoryTests.cpp"
  DEPENDS
    Fw_Types
    STest
  CHOOSES_IMPLEMENTATIONS
    "Os_File_Posix"
)

# -----------------------------------------
### Os/ConditionVariable/Posix Test Section
# -----------------------------------------

register_fprime_ut(
     PosixConditionTests 
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/condition/CommonTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/condition/ConditionRules.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixConditionTests.cpp"     
  DEPENDS
    Fw_Types
    STest
  CHOOSES_IMPLEMENTATIONS
    "Os_Mutex_Posix"
)

# -----------------------------------------
### Os/RawTime/Posix Test Section
# -----------------------------------------
register_fprime_ut(
     PosixRawTimeTest 
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/rawtime/CommonTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/../test/ut/rawtime/RawTimeRules.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PosixRawTimeTests.cpp"     
  DEPENDS
    Fw_Types
    STest
  CHOOSES_IMPLEMENTATIONS
    "Os_RawTime_Posix"
)
