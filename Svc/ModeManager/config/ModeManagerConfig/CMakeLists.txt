####
# Svc/ModeManager/config/ModeManagerConfig/CMakeLists.txt:
#
# Configuration module for Svc::ModeManager. Projects override these
# files to define their own mode set, mode graph, and transition policy.
# See Svc/ModeManager/docs/sdd.md section 4 for the override contract.
####
register_fprime_config(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/ModeManagerCfg.fpp"
        "${CMAKE_CURRENT_LIST_DIR}/ModeMachine.fpp"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/ModeManagerCfg.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/ModeList.inc"
)
