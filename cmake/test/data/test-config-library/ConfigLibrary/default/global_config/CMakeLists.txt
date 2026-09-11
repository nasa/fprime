####
# global_config/CMakeLists.txt:
#
# Library default configuration marked GLOBAL_IMPLICIT_DEPENDENCY. Consumers receive it through the
# global interface target without listing it in DEPENDS.
####
register_fprime_config(
        library_global_config
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/GlobalCfg.hpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/GlobalCfg.cpp"
    GLOBAL_IMPLICIT_DEPENDENCY
)
