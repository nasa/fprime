restrict_platforms(Posix)

register_fprime_implementation(
        Svc_FileWorker_test_FileTester
    IMPLEMENTS
        Os_File
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/FileTester.cpp"
        "${FPRIME_FRAMEWORK_PATH}/Os/Posix/FileSystem.cpp"  #TODO:mstarch:TODO
        "${FPRIME_FRAMEWORK_PATH}/Os/Posix/Directory.cpp"  #TODO:mstarch:TODO
    DEPENDS
        Fw_Types
        Fw_Time
)
