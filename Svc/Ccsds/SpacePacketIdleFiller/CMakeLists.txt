####
# F Prime CMakeLists.txt: Svc/Ccsds/SpacePacketIdleFiller
####
register_fprime_module(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/SpacePacketIdleFiller.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/SpacePacketIdleFiller.cpp"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/SpacePacketIdleFiller.hpp"
    DEPENDS
        Svc_Ccsds_Types
)

### Unit Tests ###
register_fprime_ut(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/SpacePacketIdleFiller.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/SpacePacketIdleFillerTestMain.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/SpacePacketIdleFillerTester.cpp"
    DEPENDS
        Svc_Ccsds_Types
        STest
    UT_AUTO_HELPERS
)
