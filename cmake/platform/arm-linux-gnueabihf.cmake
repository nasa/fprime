##
# arm-linux-gnueabihf.cmake:
#
# A platform file supporting the raspberry pi. This is to parallel the toolchain file
# of the same name.
##

# RPI is Linux based, so include the common Linux items
include("${CMAKE_CURRENT_LIST_DIR}/Linux-common.cmake")
