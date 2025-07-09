add_fprime_subdirectory(ComLogTSplitConfig)

register_fprime_module(
    AUTOCODER_INPUTS
        ${CMAKE_CURRENT_LIST_DIR}/ComLogTSplit.fpp
    DEPENDS
        Svc_Subtopologies_ComLogTSplit_ComLogTSplitConfig
    INTERFACE
)