####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
if (BUILD_TESTING)
    register_fprime_module(
      SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/CancelPacket.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/DataPacket.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/EndPacket.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Header.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/PathName.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/StartPacket.cpp"
      HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/FilePackets.hpp"
      DEPENDS
        gtest
        Fw_Types
        Fw_Types_GTest
        CFDP_Checksum
        CFDP_Checksum_GTest
        Fw_FilePacket
        Fw_Comp
        Fw_Port
    )
endif()
