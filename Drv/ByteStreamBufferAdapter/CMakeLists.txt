set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/ByteStreamBufferAdapter.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/ByteStreamBufferAdapter.cpp"
)

set(MOD_DEPS
    "Fw/Logger"
)

register_fprime_module()
