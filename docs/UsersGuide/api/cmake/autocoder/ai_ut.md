**Note:** auto-generated from comments in: ./autocoder/ai_ut.cmake

## autocoder/ai-ut:

Defines functions for the UT autocoder that uses AI XML inputs to generate the TesterBase and GTestBase files. This is
built on the same shared setup as the other AI autocoders, but only run when `BUILD_TESTING` is set.


## `ai_ut_is_supported`:

Required function, processes ComponentAi.xml files.
`AC_INPUT_FILE` potential input to the autocoder


## `ai_ut_setup_autocode`:

Required function, sets up a custom command to produce TesterBase and GTestBase files.


