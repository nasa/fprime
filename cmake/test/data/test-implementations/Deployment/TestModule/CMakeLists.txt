# Require some implementations
register_fprime_config(
    INTERFACE
    REQUIRES_IMPLEMENTATIONS Test_Implementation Test_Override
)

# Now create some implementations. This happens to be a convenient place, but not required to be here.
register_fprime_implementation(
        Test_Implementation_Platform
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/good_implementation.cpp"
    IMPLEMENTS
        Test_Implementation
)

register_fprime_implementation(
        Test_Implementation_Override
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/bad.cpp"
    IMPLEMENTS
        Test_Implementation
)

register_fprime_implementation(
        Test_Override_Override
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/good_override.cpp"
    IMPLEMENTS
        Test_Override
)

register_fprime_implementation(
        Test_Override_Unused
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/bad.cpp"
    IMPLEMENTS
        Test_Override
)