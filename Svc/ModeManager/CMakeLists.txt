####
# Svc/ModeManager/CMakeLists.txt:
#
# Svc::ModeManager. The mode set and mode graph live in the
# configuration module and are intended to be overridden per project.
####
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/config/ModeManagerConfig")

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/ModeManager.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/ModeManager.cpp"
  DEPENDS
    Svc_ModeManager_config_ModeManagerConfig
)

### UTs ###
# UT_AUTO_HELPERS is intentionally omitted: connectPorts()/initComponents()
# are hand-written in ModeManagerTester.cpp so that checkTransition can be
# left unconnected on purpose, exercising the shipped default policy. See
# the header comment on ModeManagerTester for why.
register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/ModeManager.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ModeManagerTester.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/ModeManagerTestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/Start.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/RequestModeCmd.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/RequestModePort.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/GetMode.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/Ping.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/ExternalPolicy.cpp"
  DEPENDS
    Svc_ModeManager_config_ModeManagerConfig
    STest
)
