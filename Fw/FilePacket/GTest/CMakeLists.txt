####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
if (BUILD_TESTING)
    set(SOURCE_FILES
      "${CMAKE_CURRENT_LIST_DIR}/CancelPacket.cpp"
      "${CMAKE_CURRENT_LIST_DIR}/DataPacket.cpp"
      "${CMAKE_CURRENT_LIST_DIR}/EndPacket.cpp"
      "${CMAKE_CURRENT_LIST_DIR}/Header.cpp"
      "${CMAKE_CURRENT_LIST_DIR}/PathName.cpp"
      "${CMAKE_CURRENT_LIST_DIR}/StartPacket.cpp"
    )

    set(MOD_DEPS
      gtest
      Fw/Cfg
      Fw/Types
      Fw/Types/GTest
      CFDP/Checksum
      CFDP/Checksum/GTest
      Fw/FilePacket
      Fw/Comp
      Fw/Port
    )
    register_fprime_module()
endif()
