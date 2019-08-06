# Default module cmake file
# AUTOCODER_INPUT_FILES: Contains all Autocoder input files
# SOURCE_FILES: Handcoded C++ source files)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/TestTopologyAppAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/TestComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/TestPortAi.xml"
)
set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/main.cpp"
)
register_fprime_module()
register_fprime_ut()


