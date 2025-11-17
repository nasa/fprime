#### CMakeLists.txt PassiveRateGroup ####

# Specifies the sources for this module with optional sources included by platform

# Specifies any module dependencies not detectable via the model file dependency detection
#

# Registers this module with the fprime build system
register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/PassiveRateGroup.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PassiveRateGroup.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/PassiveRateGroup.fpp"
  DEPENDS
    ...
)

# Specifies the sources specifically associated with unit tests in this module

# Specifies any unit test modules
#

# Registers this module with the fprime build system
register_fprime_ut(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PassiveRateGroupTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PassiveRateGroupTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PassiveRateGroup.fpp"
  DEPENDS
    ...
)
