####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/Assert.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/BasicTypes.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/EightyCharString.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/InternalInterfaceString.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/MallocAllocator.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/MemAllocator.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/PolyType.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/SerialBuffer.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Serializable.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/StringType.cpp"
)
set(MOD_DEPS
  Fw/Cfg
)
register_fprime_module()
# Add GTest directory
##TODO-MSTARCH: add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/GTest")

# Add unit test directory
##TODO-MSTARCH: add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/ut")
