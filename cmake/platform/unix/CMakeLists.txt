#set(SOURCE_FILES
#    "${CMAKE_CURRENT_LIST_DIR}/PlatformTypes.fpp"
#    "${CMAKE_CURRENT_LIST_DIR}/../../valid-empty.cpp"
#)
register_fprime_config(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/PlatformTypes.fpp"
    INTERFACE # No buildable files generated
)
#target_compile_options(${FPRIME_CURRENT_MODULE} PRIVATE
#    -Wno-unused-const-variable
#)
