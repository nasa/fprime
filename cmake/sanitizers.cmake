####
# sanitizers.cmake:
#
# Enables sanitizers in the build settings when requested by the user with -DENABLE_SANITIZER_<...>=ON.
#
# Sanitizers, by default, output their logs to stderr. To redirect the output to files instead, use the 
# `log_path` option from the sanitizer <SAN>_OPTIONS environment variable at runtime. For example, with UBSAN:
# >>> UBSAN_OPTIONS="log_path=/path/to/output_dir/file_prefix" fprime-util check
# or
# >>> UBSAN_OPTIONS="log_path=/path/to/output_dir/file_prefix" ./path/to/executable
#
# Note: <file_prefix> is a prefix to which the sanitizer will add a unique ID to generate a unique filename.
# If a relative path is specified, this will be relative to the component's folder **in the build cache** 
# if using fprime-util check, OR relative to the current directory if running a single executable.
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
            message(STATUS "[WARNING] Leak sanitizer is not supported on macOS")
        else()
            list(APPEND SANITIZERS "leak")
        endif()
    endif()

    if(ENABLE_SANITIZER_THREAD)
        if("address" IN_LIST SANITIZERS OR "leak" IN_LIST SANITIZERS)
            message(STATUS "[WARNING] Thread sanitizer does not work with Address or Leak sanitizer enabled")
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
