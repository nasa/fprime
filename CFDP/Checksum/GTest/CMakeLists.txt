####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
(CMAKE_BUILD_TYPE STREQUAL "")
    set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/Checksums.cpp"
    )

    set(MOD_DEPS
        gtest
        CFDP/Checksum
    )
    register_fprime_module()
()
