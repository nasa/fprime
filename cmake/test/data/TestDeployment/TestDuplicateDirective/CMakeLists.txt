# Repeats SOURCES in one register_fprime_* call; configuration must fail rather than drop the first list
register_fprime_library(
        TestDuplicateDirective
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/stub.cpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/stub2.cpp"
)
