####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
register_fprime_module(
    Svc_Ccsds_Interfaces
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/CcsdsSdlsDecrypt.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/CcsdsSdlsEncrypt.fpp"
  INTERFACE
)
