add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/ComLoggerTeeConfig")

register_fprime_module(
    EXCLUDE_FROM_ALL
    AUTOCODER_INPUTS
        ${CMAKE_CURRENT_LIST_DIR}/ComLoggerTee.fpp
    DEPENDS
        Svc_Subtopologies_ComLoggerTee_ComLoggerTeeConfig
    INTERFACE
)