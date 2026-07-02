set(DEPENDENCY_NAME TestDependsCollisionDependency)
set(DEPENDENCY_PATH "${CMAKE_CURRENT_LIST_DIR}/../TestDependsCollision/${DEPENDENCY_NAME}")
get_filename_component(RESOLVED_DEPENDENCY_PATH "${DEPENDENCY_PATH}" REALPATH)

register_fprime_library(
        ${DEPENDENCY_NAME}
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/stub.cpp"
)

register_fprime_library(
        TestDependsCollision
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/stub.cpp"
    DEPENDS
        ${DEPENDENCY_NAME}
        "${DEPENDENCY_PATH}"
)

get_target_property(SUPPLIED_DEPENDS TestDependsCollision SUPPLIED_DEPENDENCIES)
if (NOT SUPPLIED_DEPENDS)
    message(FATAL_ERROR "SUPPLIED_DEPENDENCIES property not set on TestDependsCollision")
endif()

list(LENGTH SUPPLIED_DEPENDS SUPPLIED_DEPENDS_LENGTH)
if (NOT SUPPLIED_DEPENDS_LENGTH EQUAL 2)
    message(FATAL_ERROR "Expected 2 SUPPLIED_DEPENDENCIES entries, got ${SUPPLIED_DEPENDS_LENGTH}")
endif()

list(GET SUPPLIED_DEPENDS 0 RELATIVE_DEPENDENCY)
if (NOT RELATIVE_DEPENDENCY STREQUAL DEPENDENCY_NAME)
    message(FATAL_ERROR "Expected relative DEPENDS entry to stay '${DEPENDENCY_NAME}', got '${RELATIVE_DEPENDENCY}'")
endif()
if (IS_ABSOLUTE "${RELATIVE_DEPENDENCY}")
    message(FATAL_ERROR "Relative DEPENDS entry was resolved to an absolute path: ${RELATIVE_DEPENDENCY}")
endif()

list(GET SUPPLIED_DEPENDS 1 ABSOLUTE_DEPENDENCY)
if (NOT ABSOLUTE_DEPENDENCY STREQUAL RESOLVED_DEPENDENCY_PATH)
    message(FATAL_ERROR "Expected absolute DEPENDS entry to resolve to '${RESOLVED_DEPENDENCY_PATH}', got '${ABSOLUTE_DEPENDENCY}'")
endif()
