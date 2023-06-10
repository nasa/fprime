# SystemResources implementation
restrict_platforms(Linux)
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/SystemResources.cpp"
)

register_fprime_module(Os_Resource_Linux)
register_fprime_implementation(Os_Resource Os_Resource_Linux)
