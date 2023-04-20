####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
if (BUILD_TESTING)
    set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/Checksums.cpp"
    )

    set(MOD_DEPS
        gtest
        CFDP/Checksum
    )
    register_fprime_module()
endif()
