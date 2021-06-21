# ====================================================================== 
# CMakeLists.txt
# ====================================================================== 

# We need to declare the XML source files this way to invoke the autocoder.
# However, only the UT build is allowed here.
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Serial1SerializableAi.xml"
)
register_fprime_module()

# Declare dependencies on test modules
set(UT_MOD_DEPS
  Fw/Test
  STest
)

# List all .cpp files as UT_SOURCE_FILES. Only the UT build is allowed.
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
)
register_fprime_ut()
