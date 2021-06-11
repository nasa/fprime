@ An assert happened
event AF_ASSERT_0(
                   file: string size 80 @< The source file of the assert
                   line: U32 @< Line number of the assert
                 ) \
  severity fatal \
  id 0 \
  format "Assert in file {}, line {}"

@ An assert happened
event AF_ASSERT_1(
                   file: string size 80 @< The source file of the assert
                   line: U32 @< Line number of the assert
                   arg1: U32 @< First assert argument
                 ) \
  severity fatal \
  id 1 \
  format "Assert in file {}, line {}: {}"

@ An assert happened
event AF_ASSERT_2(
                   file: string size 80 @< The source file of the assert
                   line: U32 @< Line number of the assert
                   arg1: U32 @< First assert argument
                   arg2: U32 @< Second assert argument
                 ) \
  severity fatal \
  id 2 \
  format "Assert in file {}, line {}: {} {}"

@ An assert happened
event AF_ASSERT_3(
                   file: string size 80 @< The source file of the assert
                   line: U32 @< Line number of the assert
                   arg1: U32 @< First assert argument
                   arg2: U32 @< Second assert argument
                   arg3: U32 @< Third assert argument
                 ) \
  severity fatal \
  id 3 \
  format "Assert in file {}, line {}: {} {} {}"

@ An assert happened
event AF_ASSERT_4(
                   file: string size 80 @< The source file of the assert
                   line: U32 @< Line number of the assert
                   arg1: U32 @< First assert argument
                   arg2: U32 @< Second assert argument
                   arg3: U32 @< Third assert argument
                   arg4: U32 @< Fourth assert argument
                 ) \
  severity fatal \
  id 4 \
  format "Assert in file {}, line {}: {} {} {} {}"

@ An assert happened
event AF_ASSERT_5(
                   file: string size 80 @< The source file of the assert
                   line: U32 @< Line number of the assert
                   arg1: U32 @< First assert argument
                   arg2: U32 @< Second assert argument
                   arg3: U32 @< Third assert argument
                   arg4: U32 @< Fourth assert argument
                   arg5: U32 @< Fifth assert argument
                 ) \
  severity fatal \
  id 5 \
  format "Assert in file {}, line {}: {} {} {} {} {}"

@ An assert happened
event AF_ASSERT_6(
                   file: string size 80 @< The source file of the assert
                   line: U32 @< Line number of the assert
                   arg1: U32 @< First assert argument
                   arg2: U32 @< Second assert argument
                   arg3: U32 @< Third assert argument
                   arg4: U32 @< Fourth assert argument
                   arg5: U32 @< Fifth assert argument
                   arg6: U32 @< Sixth assert argument
                 ) \
  severity fatal \
  id 6 \
  format "Assert in file {}, line {}: {} {} {} {} {} {}"

@ An unexpected assert happened
event AF_UNEXPECTED_ASSERT(
                            file: string size 80 @< The source file of the assert
                            line: U32 @< Line number of the assert
                            numArgs: U32 @< Number of unexpected arguments
                          ) \
  severity fatal \
  id 7 \
  format "Unexpected assert in file {}, line {}, args {}"
