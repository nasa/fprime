####
# Drv/LinuxRawEther CMakeLists
####
register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/LinuxRawEther.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/LinuxRawEther.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/LinuxRawEther.hpp"
  DEPENDS
    Os
)

### UTs ###
register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/LinuxRawEther.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/LinuxRawEtherTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/LinuxRawEtherTestMain.cpp"
)
