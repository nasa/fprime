**Note:** auto-generated from comments in: ./sanitizers.cmake

## sanitizers.cmake:

Enables sanitizers in the build settings when requested by the user with -DENABLE_SANITIZER_<...>=ON.

Sanitizers, by default, output their logs to stderr. To redirect the output to files instead, use the
`log_path` option from the sanitizer <SAN>_OPTIONS environment variable at runtime. For example, with UBSAN:
>>> UBSAN_OPTIONS="log_path=/path/to/output_dir/file_prefix" fprime-util check
or
>>> UBSAN_OPTIONS="log_path=/path/to/output_dir/file_prefix" ./path/to/executable

Note: <file_prefix> is a prefix to which the sanitizer will add a unique ID to generate a unique filename.
If a relative path is specified, this will be relative to the component's folder **in the build cache**
if using fprime-util check, OR relative to the current directory if running a single executable.


