
set(PROJECT_CONFIG_OVERRIDES
        # Test FPP override from F Prime
        "${CMAKE_CURRENT_LIST_DIR}/FpConfig.fpp"
        # Test the library source was overridden
        "${CMAKE_CURRENT_LIST_DIR}/config.cpp"
)
# Dropping the header override lets a test check that the destination goes back
# to the file it was shadowing
if (NOT _TEST_CONFIG_DROP_HEADER_OVERRIDE)
    list(PREPEND PROJECT_CONFIG_OVERRIDES
        # Test header override from F Prime
        "${CMAKE_CURRENT_LIST_DIR}/DpCfg.hpp"
    )
endif()

register_fprime_config(
        project_config
    CONFIGURATION_OVERRIDES
        ${PROJECT_CONFIG_OVERRIDES}
    INTERFACE
)
# Special config to test bad overrides
if (_TEST_CONFIG_BAD_OVERRIDE)
    register_fprime_config(
            project_config_bad_override
        HEADERS
            # Test new header that accidentally overrides F Prime
            "${CMAKE_CURRENT_LIST_DIR}/DpCfg.hpp"
        INTERFACE
    )
endif()
# Special config to test bad overrides
if (_TEST_CONFIG_BAD_NEW_FILE)
    register_fprime_config(
            project_config_bad_new_config
        CONFIGURATION_OVERRIDES
            # Test header override from F Prime
            "${CMAKE_CURRENT_LIST_DIR}/DoesNotExist.hpp"
        INTERFACE
    )
endif()