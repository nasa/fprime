set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/topology.fpp"
)

set(MOD_DEPS
        ${PROJECT_NAME}/topology/ports
        ${PROJECT_NAME}/topology/components/Comp
        ${PROJECT_NAME}/topology/components/Framework
)

register_fprime_module()

# Declare dependencies on test modules
set(UT_MOD_DEPS
        STest
)

set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
)
register_fprime_ut()
