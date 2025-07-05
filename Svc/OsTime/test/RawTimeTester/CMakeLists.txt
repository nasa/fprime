register_fprime_implementation(
        Svc_OsTime_test_RawTimeTester
    IMPLEMENTS
        Os_RawTime
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/RawTimeTester.cpp"
    DEPENDS
        Fw_Types
        Fw_Time
)

