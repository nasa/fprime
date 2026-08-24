####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
####
add_compile_options(
  -Wno-shadow
)

# The topology's text event connections need CdhCore.textLogger, which does
# not exist when FPRIME_ENABLE_TEXT_LOGGERS is off. Select the matching
# topology variant.
if (FPRIME_ENABLE_TEXT_LOGGERS)
  set(REF_TOPOLOGY_FPP "${CMAKE_CURRENT_LIST_DIR}/topology.fpp")
else()
  set(REF_TOPOLOGY_FPP "${CMAKE_CURRENT_LIST_DIR}/topology-no-text-logger.fpp")
endif()

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/instances.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/system.fpp"
    "${REF_TOPOLOGY_FPP}"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/RefTopology.cpp"
  DEPENDS
    Fw_Logger
    Svc_PosixTime
    Svc_TlmPacketizer_config_TlmPacketizerConfig # Supports packet autocoding w/o TlmPacketizer  
)
