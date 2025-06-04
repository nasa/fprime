set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/CDHCore.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/intentionally-empty.cpp"
)

register_fprime_module()

# Set the module name explicitly to match what the build system expects
set_target_properties(
    ${FPRIME_CURRENT_MODULE}
    PROPERTIES
    SOURCES "${CMAKE_CURRENT_LIST_DIR}/intentionally-empty.cpp"
)
