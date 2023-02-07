# Porting Guide

Porting F′ to a new hardware platform is often required for using F´ on
new missions and projects. Fortunately, F´ is designed to be mostly
independent of hardware. There are four parts of F´ that need to be
developed to run on a new platform:

1.  CMake Toolchain and F´ platform files. These files specify the
    location of compilers and other build tools for compiling F´ for a
    given platform.

2.  Fw::Types and configuration. A Fw::Types header may be needed along with appropriate configuration if the hardware needs settings that are different from the default.

3.  Hardware drivers. Any drivers that are outside the provided Linux
    Driver package would need to be developed for the hardware-specific
    interfaces to be supported.

4.  OSAL support. Should a non-Linux operating system be needed to run
    on the platform, OSAL support files must be developed. See: [OS File Descriptions](./os-docs.md)

Once these areas are implemented, the user should be able to run on a new system or platform.