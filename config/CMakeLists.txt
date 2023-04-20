####
# config/CMakeLists.txt:
#
# Sets a list of source files for cmake to process as part of autocoding.
####
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/FpConfig.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/AcConstants.fpp"
)
register_fprime_module(config)
