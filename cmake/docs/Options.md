**Note:** auto-generated from comments in: ../Options.cmake

## Command Line Options:

Options used to configure F prime's CMake system. These options control various actions available
in the CMake system to help users achieve specific results. Each options has a sensible default
such that the user need not worry about specifing each correctly.

Typically a user need not specified any of the options to build F prime, however; when
non-standard build behavior is desired, then these options can be used. These options are
specified with the -D<OPTION>=<VALUE> flag. Usually the value is "ON" or "OFF". Other values are
noted if the option differs.

**Non-standard behavior:**

- Build with build system debigging messages
- Generate autocoding files in-source
- Build and link with shared libraries
- Generate heritage python dictionaries
- Manually specify the build platform

@author mstarch


## `CMAKE_DEBUG_OUTPUT:`

Turns on the reporting of debug output of the CMake build. Can help refine the CMake system,
and repair errors. For normal usage, this is not necessary.

**Values:**
- ON: generate debugging output
- OFF: (default) do *not* generate debugging output

e.g. `-DCMAKE_DEBUG_OUTPUT=ON`


## `GENERATE_AC_IN_SOURCE:`

Allows for generating auto-coded files in the source tree. Typically, this is frowned upon as
this prevents using F prime as a library. However, this is necessary to support historic
project development practices.

**WARNING:** due to the limitations of the autocoder, currently the CMake system generates
             in-source and moves the files out.

**Values:**
- ON: generate auto-coding files in source-tree.
- OFF: (default) generate auto-coding files in the build-tree.

e.g. `-DGENERATE_AC_IN_SOURCE=ON`


## `LINK_AS_SHARED_LIBS:`

This option swiches the default link option from static linkage to using a shared-object linkage.
It implies that instead of static archive files, shared objects will be built instead. This will
mean smaller binaries, partial-updates are possible, and more rigorous project process must be
used.

**Values:**
- ON: generate shared libraries with shared-linkage.
- OFF: (default) generate static libraries with static-linkage.

e.g. `-DLINK_AS_SHARED_LIBS=ON`


## `GENERATE_HERITAGE_PY_DICT:`

This option switches from generating XML dictionaries to generating the heritage python
dictionatries. This enables backward compatible use with the older Tk GUI and other tools that
use python fragment dictionaries.

**Values:**
- ON: generate python dictionaries.
- OFF: (default) generate XML dictionaries.

e.g. `-DGENERATE_HERITAGE_PY_DICT=ON`


## `PLATFORM:`

Specifies the platform used when building the F prime using the CMake system. See:
[platform.md](platform.md) for more information.


