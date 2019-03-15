####
# empty.c:
#
# In order to build some targets, an empty .c file must be created and included in the
# source list. This portion of the cmake system generates the empty.c file.
#
# This file is generated in this directory for use in multiple parts of the system.
####
set(EMPTY_C_SRC "${CMAKE_CURRENT_LIST_DIR}/empty.c")
file(WRITE "${EMPTY_C_SRC}" "#define CMAKE_EMPTY_SOURCE\n")

# Add empty source to cache
set(EMPTY_C_SRC "${EMPTY_C_SRC}" CACHE INTERNAL "Empty c source file")
