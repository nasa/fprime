#### CMakeLists.txt PassiveRateGroup ####

# Specifies the sources for this module with optional sources included by platform
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/PassiveRateGroup.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PassiveRateGroup.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/PassiveRateGroup.fpp"
)


# Specifies any module dependencies not detectable via the model file dependency detection
#set(MOD_DEPS ...)

# Registers this module with the fprime build system
register_fprime_module()

# Specifies the sources specifically associated with unit tests in this module
set(UT_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PassiveRateGroupTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/PassiveRateGroupImplTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PassiveRateGroup.fpp"
)


# Specifies any unit test modules
#set(UT_MOD_DEPS ...)

# Registers this module with the fprime build system
register_fprime_ut()
