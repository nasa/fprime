####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####

####
# WARNING: not for use outside of fprime/Os
#
# Adds a named os module (e.g. common code for an OSAL implementation). Helper to allow multiple commons per file.
#
# NAMES: list of named files to add to this module.  The first will be treated as the name of the module.
#        i.e. File;Directory;FileSystem will contain the file, directory, and filesystem files in a module called File.
# ARGN: extra MOD_DEPS to add
####
function(add_named_os_module NAMES)
    # Get first element of list
    list(GET NAMES 0 FIRST_ITEM)
    set(OS_MODULE_NAME "Os_${FIRST_ITEM}")
    # Clear variables
    set(SOURCE_INPUTS)
    set(HEADER_INPUTS)
    # Add files for each name supplied
    foreach (NAME IN LISTS NAMES)
        list(APPEND SOURCE_INPUTS "${CMAKE_CURRENT_LIST_DIR}/${NAME}.cpp")
        list(APPEND HEADER_INPUTS "${CMAKE_CURRENT_LIST_DIR}/${NAME}.hpp")
    endforeach ()
    # Set up module
    register_fprime_module(
          "${OS_MODULE_NAME}"
        REQUIRES_IMPLEMENTATIONS
          "Os_${FIRST_ITEM}"
        SOURCES
          ${SOURCE_INPUTS}
        HEADERS
          ${HEADER_INPUTS}
        DEPENDS
          Fw_Time
          Fw_Types ${ARGN}
    )
    fprime_target_dependencies(Os PUBLIC "${OS_MODULE_NAME}")
endfunction()

####
# WARNING: not for use outside of fprime/Os
#
# Adds a named os implementation supplied by fprime core. Helper to allow multiple implementations per file.
#
# NAMES: list of named files to add to this module.  The first will be treated as the name of the module.
#        i.e. File;Directory;FileSystem will contain the file, directory, and filesystem files in a module called File.
# SUFFIX: suffix to implementation (e.g. Posix)
# ARGN: extra MOD_DEPS to add
####
function(add_fprime_supplied_os_module NAMES SUFFIX)
    # Get first element of list
    list(GET NAMES 0 FIRST_ITEM)

    # Ensure expected files with expected filenames exist
    if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/Default${FIRST_ITEM}.cpp")
        message(FATAL_ERROR "${CMAKE_CURRENT_LIST_DIR}/Default${FIRST_ITEM}.cpp does not exist.")
    endif()

    foreach (NAME IN LISTS NAMES)
        if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/${NAME}.cpp")
            message(FATAL_ERROR "${CMAKE_CURRENT_LIST_DIR}/${NAME}.cpp does not exist.")
        endif()
        if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/${NAME}.hpp")
            message(FATAL_ERROR "${CMAKE_CURRENT_LIST_DIR}/${NAME}.hpp does not exist.")
        endif()
    endforeach()

    # Setup variables
    set(OS_MODULE_NAME "Os_${FIRST_ITEM}_${SUFFIX}")
    # Clear variables
    set(SOURCE_INPUTS)
    set(HEADERS_INPUTS)
    # Add files for each name supplied
    foreach (NAME IN LISTS NAMES)
        list(APPEND SOURCE_INPUTS "${CMAKE_CURRENT_LIST_DIR}/${NAME}.cpp")
        list(APPEND HEADERS_INPUTS "${CMAKE_CURRENT_LIST_DIR}/${NAME}.hpp")
    endforeach ()
    # Set up module
    register_fprime_module(
        "${OS_MODULE_NAME}_Implementation"
      SOURCES
        ${SOURCE_INPUTS}
      HEADERS
        ${HEADER_INPUTS}
      DEPENDS
        Fw_Time
        Fw_Types
        "Os_${FIRST_ITEM}"
        ${ARGN}
    )
    register_fprime_implementation(
         "${OS_MODULE_NAME}"
      SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/Default${FIRST_ITEM}.cpp"
      IMPLEMENTS
        "Os_${FIRST_ITEM}"
      DEPENDS
        Fw_Time
        Fw_Types
        "${OS_MODULE_NAME}_Implementation"
    )
endfunction()

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Models")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Stub")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Posix")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Generic")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Linux")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Darwin")

register_fprime_module(
      Os
    AUTOCODER_INPUTS
      "${CMAKE_CURRENT_LIST_DIR}/Types.fpp"
    SOURCES
      "${CMAKE_CURRENT_LIST_DIR}/ValidateFileCommon.cpp"
      "${CMAKE_CURRENT_LIST_DIR}/ValidatedFile.cpp"
      "${CMAKE_CURRENT_LIST_DIR}/IntervalTimer.cpp"
      "${CMAKE_CURRENT_LIST_DIR}/Os.cpp"
    HEADERS
      "${CMAKE_CURRENT_LIST_DIR}/ValidatedFile.hpp"
      "${CMAKE_CURRENT_LIST_DIR}/Os.hpp"
    DEPENDS
      Fw_Time
      Fw_Types
)

add_named_os_module(Console Fw_Logger)
add_named_os_module("File;FileSystem;Directory" Utils_Hash)
add_named_os_module(Task ${CMAKE_THREAD_LIBS_INIT})
add_named_os_module("Mutex;Condition")
add_named_os_module(Queue)
add_named_os_module(Cpu)
add_named_os_module(Memory)
add_named_os_module(RawTime Fw_Buffer)

### UTS ### Note: 3 separate UTs registered here.
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/OsTestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/IntervalTimerTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/OsValidateFileTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/OsMutexBasicLockableTest.cpp"
)
register_fprime_ut()

if (BUILD_TESTING)
    set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/file/SyntheticFileSystem.cpp"
    )
    set(MOD_DEPS Fw_Types)
    register_fprime_module(Os_Test_File_SyntheticFileSystem)
endif()
