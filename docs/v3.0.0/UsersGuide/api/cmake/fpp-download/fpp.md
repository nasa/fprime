**Note:** auto-generated from comments in: ./fpp-download/fpp.cmake

## fpp.cmake:

A setup to install fpp tool suite automatically as part of the CMake run. If the user wants to avoid this, the user
should install FPP on the system path and that will be used.


## locate_fpp_tools:

Locates the fpp tool suite and sets FPP_FOUND if the right version of the tools is found. It will look first to the
above install location and then to the system path as a fallback.


