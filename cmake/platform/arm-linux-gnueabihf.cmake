##
# arm-linux-gnueabihf.cmake:
#
# A platform file supporting the raspberry pi. This is the F prime platform support file that combines with the
# toolchain file found at [../toolchain/raspberrypi.cmake](../toolchain/raspberrypi.cmake).
##

# RPI is Linux based, so include the common Linux items
include("${CMAKE_CURRENT_LIST_DIR}/Linux-common.cmake")
