# SystemResources implementation
restrict_platforms(Darwin)
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/SystemResources.cpp"
)

register_fprime_module(Os_Resource_Darwin)
register_fprime_implementation(Os_Resource Os_Resource_Darwin)
