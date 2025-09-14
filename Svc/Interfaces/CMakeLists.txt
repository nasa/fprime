####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
register_fprime_module(
    Svc_Interfaces
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/Com.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/Deframer.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/FrameAccumulator.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/Framer.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/Router.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/Time.fpp"
  INTERFACE
)
