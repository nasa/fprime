**Note:** auto-generated from comments in: ./sanitizers.cmake

## sanitizers.cmake:

Enables sanitizers in the build settings when requested by the user with -DENABLE_SANITIZER_<...>=ON.

Sanitizers, by default, output their logs to stderr. To redirect the output to files instead, use the
`log_path` option from the sanitizer <SAN>_OPTION environment variable at runtime. For example, with UBSAN:
>>> UBSAN_OPTIONS="log_path=/path/to/output_dir" fprime-util check   (or a single executable file).
If a relative path is specified, this will be relative to the test main function's file **in the build cache**.


