####
# sanitizers.cmake:
#
# Enables sanitizers in the build settings when requested by the user with -DENABLE_SANITIZER_<...>=ON.
#
# Sanitizers, by default, output their logs to stderr. To redirect the output to files instead, use the 
# `log_path` option from the sanitizer <SAN>_OPTION environment variable at runtime. For example, with UBSAN:
# >>> UBSAN_OPTIONS="log_path=/path/to/output_dir" fprime-util check   (or a single executable file).
# If a relative path is specified, this will be relative to the test main function's file **in the build cache**.
####

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    set(SANITIZERS)

    if(ENABLE_SANITIZER_ADDRESS)
        list(APPEND SANITIZERS "address")
    endif()

    if(ENABLE_SANITIZER_UNDEFINED_BEHAVIOR)
        list(APPEND SANITIZERS "undefined")
    endif()

    if(ENABLE_SANITIZER_LEAK)
        if(APPLE) 
            message(WARNING "Leak sanitizer not supported on macOS.")
        else()
            list(APPEND SANITIZERS "leak")
        endif()
    endif()

    if(ENABLE_SANITIZER_THREAD)
        if("address" IN_LIST SANITIZERS OR "leak" IN_LIST SANITIZERS)
            message(WARNING "Thread sanitizer does not work with Address or Leak sanitizer enabled")
        else()
            list(APPEND SANITIZERS "thread")
        endif()
    endif()

    list(JOIN SANITIZERS "," LIST_OF_SANITIZERS)

    if(LIST_OF_SANITIZERS AND NOT "${LIST_OF_SANITIZERS}" STREQUAL "")
            message(STATUS "Enabled the following sanitizers: ${LIST_OF_SANITIZERS}")
            add_compile_options(-fsanitize=${LIST_OF_SANITIZERS})
            add_link_options(-fsanitize=${LIST_OF_SANITIZERS})
    endif()
endif()
