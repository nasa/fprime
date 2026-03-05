
register_os_implementation("File;FileSystem;Directory" Multi)

register_fprime_module(
    Os_Generic_OsalRegistry
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/OsalRegistry.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/OsalRegistry.hpp"
  DEPENDS
    Fw_Types
)

# register_fprime_implementation(
#     Os_Generic_MultiFileSystem
#   SOURCES
#     "${CMAKE_CURRENT_LIST_DIR}/DefaultFileSystem.cpp"
#   IMPLEMENTS
#     Os_FileSystem
#   DEPENDS
#     Fw_Types
#     Os_Generic_MultiFileSystem_Implementation
# )

# register_fprime_ut(
#     MultiFileSystemTest
#   SOURCES
#     "${CMAKE_CURRENT_LIST_DIR}/test/ut/MultiFileSystemTests.cpp"
#     "${CMAKE_CURRENT_LIST_DIR}/../test/ut/filesystem/CommonTests.cpp"
#     "${CMAKE_CURRENT_LIST_DIR}/../test/ut/filesystem/FilesystemRules.cpp"
#   DEPENDS
#     Fw_Types
#     Fw_Time
#     Os
#     STest
#   CHOOSES_IMPLEMENTATIONS
#     Os_Generic_MultiFileSystem
# )
