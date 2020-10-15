**Note:** auto-generated from comments in: ./toolchain/raspberrypi.cmake

## Raspberry PI Toolchain

A toolchain file dor the raspberrypi. This toolchain can be used to build against the raspberry pi embedded Linux
target. In order to use this toolchain, the raspberry pi build tools should be cloned onto a Linux build host. These
tools are available at: [https://github.com/raspberrypi/tools](https://github.com/raspberrypi/tools)

Typically these tools are cloned to `/opt/rpi/`.  If they are cloned elsewhere, the user must set the environment
variable `RPI_TOOLCHAIN_DIR` to the full path to the `arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf` directory before
running CMake or `fprime-util generate`.

e.g. should the user install the tools in ``/home/user1` then the environment variable might be set using
`export RPI_TOOLCHAIN_DIR=/home/user/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/`

**Note:** this toolchain file sets the system name to `arm-linux-gnueabihf` to be similar to other raspberry pi
toolchain files while providing a cleaner name of `raspberrypi`.  Its parallel platform file is therefore named
`arm-linux-gnueabihf.cmake` in the platforms directory.


