####
# Options:
#
# Options used to configure F prime's CMake system. These options control various actions available
# in the CMake system to help users achieve specific results. Each Option added *must* supply a
# sensible default, comment defining the behaviro, and should be ON/OFF as is common with CMake
# options.
####

####
# CMAKE_DEBUG_OUTPUT:
#
# Turns on the reporting of debug output of the CMake build. Can help refine the CMake system,
# and repair errors. For normal usage, this is not necessary.
####
option(CMAKE_DEBUG_OUTPUT "Generate F prime's debug output while running CMake" OFF)

####
# GENERATE_AC_IN_SOURCE:
#
# Allows for generating auto-coded files in the source tree. Typically, this is frowned upon as
# this prevents using F prime as a library. However, this is necessary to support historic
# project development practices.
####
option(GENERATE_AC_IN_SOURCE "Ac.cpp and Ac.hpp are placed with source files." OFF)

####
# LINK_AS_SHARED_LIBS:
#
# This option swiches the default link option from static linkage to using a shared-object linkage.
# It implies that instead of static archive files, shared objects will be generated instead. This
# will mean smaller binaries, and partial-updates but implies less rigorous control over the
# runtime behavior.
####
option(LINK_AS_SHARED_LIBS "Link F prime with shared libraries." OFF)
