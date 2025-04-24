####
# config/CMakeLists.txt:
#
# Sets a list of source files for cmake to process as part of autocoding.
####
register_fprime_module(
        config
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/AcConstants.fpp"
        "${CMAKE_CURRENT_LIST_DIR}/DpCfg.fpp"
        "${CMAKE_CURRENT_LIST_DIR}/FpConfig.fpp"
        "${CMAKE_CURRENT_LIST_DIR}/PolyDbCfg.fpp"
        "${CMAKE_CURRENT_LIST_DIR}/VersionCfg.fpp"
        "${CMAKE_CURRENT_LIST_DIR}/FpySequencerCfg.fpp"
)
