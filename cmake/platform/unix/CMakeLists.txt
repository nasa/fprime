set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/PlatformTypes.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/../../valid-empty.cpp"
)
register_fprime_module()
target_compile_options(${FPRIME_CURRENT_MODULE} PRIVATE
    -Wno-unused-const-variable
)
