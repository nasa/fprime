# Build some module
set(SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/Empty.cpp")
register_fprime_module()
# Require some implementations
require_fprime_implementation(Test/Implementation)
require_fprime_implementation(Test/Override)

# Now create some implementations. This happens to be a convenient place, but not required to be here.

set(SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/Empty.cpp")
register_fprime_module(Test_Implementation_Platform)

set(SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/Empty.cpp")
register_fprime_module(Test_Implementation_Override)

set(SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/Empty.cpp")
register_fprime_module(Test_Override_Override)