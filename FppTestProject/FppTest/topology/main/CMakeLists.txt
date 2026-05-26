set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/instances.fpp"
        "${CMAKE_CURRENT_LIST_DIR}/topology.fpp"
)

set(MOD_DEPS
        ${PROJECT_NAME}/topology/ports
        ${PROJECT_NAME}/topology/components/Sender
        ${PROJECT_NAME}/topology/components/Receiver
)

register_fprime_module()

# Declare dependencies on test modules
set(UT_MOD_DEPS STest)

set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/main.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/../../component/types/FormalParamTypes.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/../../utils/Utils.cpp"
)
register_fprime_ut()
