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
    set(OS_MODULE_NAME "Os_${FIRST_ITEM}_Common")
    # Clear variables
    set(SOURCE_FILES)
    set(SOURCE_HEADERS)
    # Add files for each name supplied
    foreach (NAME IN LISTS NAMES)
        list(APPEND SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/${NAME}.cpp")
        list(APPEND SOURCE_HEADERS "${CMAKE_CURRENT_LIST_DIR}/${NAME}.hpp")
    endforeach ()
    # Set up module
    set(MOD_DEPS Fw_Types ${ARGN})
    require_fprime_implementation("Os_${FIRST_ITEM}")
    register_fprime_module("${OS_MODULE_NAME}")
    add_dependencies(Os "${OS_MODULE_NAME}")
    target_link_libraries(Os PUBLIC "${OS_MODULE_NAME}")
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
    set(OS_MODULE_NAME "Os_${FIRST_ITEM}_${SUFFIX}")
    # Clear variables
    set(SOURCE_FILES)
    set(SOURCE_HEADERS)
    # Add files for each name supplied
    foreach (NAME IN LISTS NAMES)
        list(APPEND SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/${NAME}.cpp")
        list(APPEND SOURCE_HEADERS "${CMAKE_CURRENT_LIST_DIR}/${NAME}.hpp")
    endforeach ()
    # Set up module
    set(MOD_DEPS "Os_${FIRST_ITEM}_Common" ${ARGN})
    register_fprime_module("${OS_MODULE_NAME}")
    register_fprime_implementation("Os_${FIRST_ITEM}" "${OS_MODULE_NAME}" "${CMAKE_CURRENT_LIST_DIR}/Default${FIRST_ITEM}.cpp")
endfunction()

####
# WARNING: not for use outside of fprime/Os
#
# Adds a test for OSAL layer. Helper to allow multiple tests per file.
#
# NAME: Name of test
# UT_SOURCE_FILES: list of source files
# IMPLEMENTATION_PAIRS: a list of pairs of implementation choices.  e.g. "Os_File\;Os_File_Posix;Os_Task\;Os_Task_Stub"
#                       pairs are represented with internal escaped list separators.
# ARGN: extra MOD_DEPS to add
####
function(add_fprime_os_test NAME UT_SOURCE_FILES IMPLEMENTATION_PAIRS)
    set(UT_MOD_DEPS Os STest ${ARGN})
    foreach (PAIR IN LISTS IMPLEMENTATION_PAIRS)
        choose_fprime_implementation(${PAIR})
    endforeach ()
    register_fprime_ut("${NAME}")
endfunction()

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

# Basic source files used in every OSAL layer.  Contains common code and helpers.
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/IntervalTimerCommon.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Posix/IntervalTimer.cpp"

    "${CMAKE_CURRENT_LIST_DIR}/ValidateFileCommon.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ValidatedFile.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Os.cpp"
)
set(SOURCE_HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/Posix/IntervalTimer.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/ValidatedFile.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Os.hpp"
)
set(MOD_DEPS Fw_Types)
register_fprime_module()

add_named_os_module(Console Fw_Logger)
add_named_os_module("File;FileSystem;Directory" Utils/Hash)
add_named_os_module(Task ${CMAKE_THREAD_LIBS_INIT})
add_named_os_module("Mutex;Condition")
add_named_os_module(Queue)
add_named_os_module(Cpu)
add_named_os_module(Memory)

### UTS ### Note: 3 separate UTs registered here.
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/OsTestMain.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/IntervalTimerTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/OsValidateFileTest.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/OsMutexBasicLockableTest.cpp"
)
register_fprime_ut()
