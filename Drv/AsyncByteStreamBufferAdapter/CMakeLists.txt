set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/AsyncByteStreamBufferAdapter.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/AsyncByteStreamBufferAdapter.cpp"
)

set(MOD_DEPS
    "Fw/Logger"
)

register_fprime_module()
