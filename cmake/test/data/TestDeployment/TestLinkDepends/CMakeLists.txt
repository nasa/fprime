register_fprime_library(
        TestLinkDepends
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/stub.cpp"
    LINK_DEPENDS
        "${CMAKE_CURRENT_LIST_DIR}/test_link_dep.ld"
        "${CMAKE_CURRENT_LIST_DIR}/test_link_dep2.ld"
)

# Verify LINK_DEPENDS property was set on the target
get_target_property(LINK_DEPS TestLinkDepends LINK_DEPENDS)
if (NOT LINK_DEPS)
    message(FATAL_ERROR "LINK_DEPENDS property not set on TestLinkDepends")
endif()

list(LENGTH LINK_DEPS LINK_DEPS_LENGTH)
if (NOT LINK_DEPS_LENGTH EQUAL 2)
    message(FATAL_ERROR "Expected 2 LINK_DEPENDS entries, got ${LINK_DEPS_LENGTH}")
endif()

foreach(DEP IN LISTS LINK_DEPS)
    if (NOT EXISTS "${DEP}")
        message(FATAL_ERROR "LINK_DEPENDS entry does not exist: ${DEP}")
    endif()
endforeach()
